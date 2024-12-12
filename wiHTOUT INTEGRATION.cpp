//
// pch.h
//

#pragma once
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<filesystem>
#include <cstdio>
#include<queue>
#include "BigInt.h"
using namespace std;
class Node1;
class RoutingTable;
bool deleteFile(const std::string& filePath) {
    // Convert string to c-string
    const char* filePathCStr = filePath.c_str();

    // Delete the file
    if (std::remove(filePathCStr) != 0) {
        std::cout << "Error deleting the file: " << filePath << std::endl;
        return false;
    }

    std::cout << "File deleted successfully: " << filePath << std::endl;
    return true;
}
void copyFileToMachineFolder(const std::string& sourceFilePath, const std::string& machineFolder);
//MAke b-tree implementation which keys also have a value and I will store the value in the node
template <class T, class L>
class Node {
public:
    T* keys;
    L* values;
    int t;
    Node<T, L>** children;
    int n;
    bool leaf;
public:
    Node(int t, bool leaf) {
        this->t = t;
        this->leaf = leaf;
        keys = new T[2 * t - 1];
        values = new L[2 * t - 1];
        children = new Node<T, L>* [2 * t];
        n = 0;
    }
    void insertNonFull(T key, L value, const string& machineFolder) {
        int i = n - 1;
        if (leaf) {
            while (i >= 0 && keys[i] > key) {
                keys[i + 1] = keys[i];
                values[i + 1] = values[i];
                i--;
            }
            keys[i + 1] = key;
            values[i + 1] = value;
            n++;

            // Copy the file to the machine folder
            copyFileToMachineFolder(value, machineFolder);
        }
        else {
            while (i >= 0 && keys[i] > key) {
                i--;
            }
            if (children[i + 1]->n == 2 * t - 1) {
                splitChild(i + 1, children[i + 1]);
                if (keys[i + 1] < key) {
                    i++;
                }
            }
            children[i + 1]->insertNonFull(key, value, machineFolder);
        }
    }
    void splitChild(int i, Node<T, L>* y) {
        Node<T, L>* z = new Node<T, L>(y->t, y->leaf);
        z->n = t - 1;
        for (int j = 0; j < t - 1; j++) {
            z->keys[j] = y->keys[j + t];
            z->values[j] = y->values[j + t];
        }
        if (!y->leaf) {
            for (int j = 0; j < t; j++) {
                z->children[j] = y->children[j + t];
            }
        }
        y->n = t - 1;
        for (int j = n; j >= i + 1; j--) {
            children[j + 1] = children[j];
        }
        children[i + 1] = z;
        for (int j = n - 1; j >= i; j--) {
            keys[j + 1] = keys[j];
            values[j + 1] = values[j];
        }
        keys[i] = y->keys[t - 1];
        values[i] = y->values[t - 1];
        n++;
    }
    void traverse() {
        int i;
        for (i = 0; i < n; i++) {
            if (!leaf) {
                children[i]->traverse();
            }
            cout << keys[i] << " ";
        }
        if (!leaf) {
            children[i]->traverse();
        }
    }
    Node<T, L>* search(T key) {
        int i = 0;
        while (i < n && keys[i] < key) {
            i++;
        }
        if (keys[i] == key) {
            return this;
        }
        if (leaf) {
            return NULL;
        }
        return children[i]->search(key);
    }
    void remove(T key, string machineFolder, string& res) {
        int i = 0;
        while (i < n && keys[i] < key) {
            i++;
        }

        // Find the value corresponding to the key being deleted
        string deletedValue = "";
        if (keys[i] == key) {
            res = values[i];
            filesystem::path path(values[i]);
            deletedValue = path.filename().string();
        }

        if (leaf) {
            for (int j = i; j < n - 1; j++) {
                keys[j] = keys[j + 1];
                values[j] = values[j + 1];
            }
            n--;

            // Delete the corresponding file from the machine folder
            if (!deletedValue.empty()) {
                string deletedFilePath = machineFolder + "/" + deletedValue;
                deleteFile(deletedFilePath);
            }
        }
        else {
            if (children[i]->n >= t) {
                T pred = children[i]->getPred(i);
                keys[i] = pred;
                values[i] = children[i]->getPredValue(i);
                children[i]->remove(pred, machineFolder, res);
            }
            else if (children[i + 1]->n >= t) {
                T succ = children[i + 1]->getSucc(i);
                keys[i] = succ;
                values[i] = children[i + 1]->getSuccValue(i);
                children[i + 1]->remove(succ, machineFolder, res);
            }
            else {
                merge(i);
                children[i]->remove(key, machineFolder, res);
            }
        }
    }
    T getPred(int i) {
        Node<T, L>* cur = children[i];
        while (!cur->leaf) {
            cur = cur->children[cur->n];
        }
        return cur->keys[cur->n - 1];
    }
    L getPredValue(int i) {
        Node<T, L>* cur = children[i];
        while (!cur->leaf) {
            cur = cur->children[cur->n];
        }
        return cur->values[cur->n - 1];
    }
    T getSucc(int i) {
        Node<T, L>* cur = children[i + 1];
        while (!cur->leaf) {
            cur = cur->children[0];
        }
        return cur->keys[0];
    }
    L getSuccValue(int i) {
        Node<T, L>* cur = children[i + 1];
        while (!cur->leaf) {
            cur = cur->children[0];
        }
        return cur->values[0];
    }
    void fill(int i) {
        if (i != 0 && children[i - 1]->n >= t) {
            borrowFromPrev(i);
        }
        else if (i != n && children[i + 1]->n >= t) {
            borrowFromNext(i);
        }
        else {
            if (i != n) {
                merge(i);
            }
            else {
                merge(i - 1);
            }
        }
    }
    void borrowFromPrev(int i) {
        Node<T, L>* child = children[i];
        Node<T, L>* sibling = children[i - 1];
        for (int j = child->n - 1; j >= 0; j--) {
            child->keys[j + 1] = child->keys[j];
            child->values[j + 1] = child->values[j];
        }
        if (!child->leaf) {
            for (int j = child->n; j >= 0; j--) {
                child->children[j + 1] = child->children[j];
            }
        }
        child->keys[0] = keys[i - 1];
        child->values[0] = values[i - 1];
        if (!child->leaf) {
            child->children[0] = sibling->children[sibling->n];
        }
        keys[i - 1] = sibling->keys[sibling->n - 1];
        values[i - 1] = sibling->values[sibling->n - 1];
        child->n++;
        sibling->n--;
    }
    void borrowFromNext(int i) {
        Node<T, L>* child = children[i];
        Node<T, L>* sibling = children[i + 1];
        child->keys[child->n] = keys[i];
        child->values[child->n] = values[i];
        if (!child->leaf) {
            child->children[child->n + 1] = sibling->children[0];
        }
        keys[i] = sibling->keys[0];
        values[i] = sibling->values[0];
        for (int j = 1; j < sibling->n; j++) {
            sibling->keys[j - 1] = sibling->keys[j];
            sibling->values[j - 1] = sibling->values[j];
        }
        if (!sibling->leaf) {
            for (int j = 1; j <= sibling->n; j++) {
                sibling->children[j - 1] = sibling->children[j];
            }
        }
        child->n++;
        sibling->n--;
    }
    void merge(int i) {
        Node<T, L>* child = children[i];
        Node<T, L>* sibling = children[i + 1];
        child->keys[t - 1] = keys[i];
        child->values[t - 1] = values[i];
        for (int j = 0; j < sibling->n; j++) {
            child->keys[j + t] = sibling->keys[j];
            child->values[j + t] = sibling->values[j];
        }
        if (!child->leaf) {
            for (int j = 0; j <= sibling->n; j++) {
                child->children[j + t] = sibling->children[j];
            }
        }
        for (int j = i + 1; j < n; j++) {
            keys[j - 1] = keys[j];
            values[j - 1] = values[j];
        }
        for (int j = i + 2; j <= n; j++) {
            children[j - 1] = children[j];
        }
        child->n += sibling->n + 1;
        n--;
        delete(sibling);
    }
    void print() {
        for (int i = 0; i < n; i++) {
            cout << " (" << keys[i] << " " << values[i] << ")  ";
        }
        cout << endl;
    }
    void printTree() {
        print();
        if (!leaf) {
            for (int i = 0; i <= n; i++) {
                children[i]->printTree();
            }
        }
    }
    string searchvalue(T key, string s) {
        int i = 0;
        while (i < n && keys[i] < key) {
            i++;
        }
        if (keys[i] == key) {
            stringstream ss;
            ss << values[i];
            s += ss.str();
            return s;
        }
        if (leaf) {
            return "Key not found";
        }
        return children[i]->searchvalue(key, s);
    }
};
template <class T, class L>
class Btree {
    Node<T, L>* root;
    int t;
public:
    Btree(int t) {
        this->t = t;
        root = NULL;
    }
    void insert(T key, L value, const string& machineFolder) {
        if (root == NULL) {
            root = new Node<T, L>(t, true);
            root->keys[0] = key;
            root->values[0] = value;
            root->n = 1;
            copyFileToMachineFolder(root->values[0], machineFolder);
        }
        else {
            if (root->n == 2 * t - 1) {
                Node<T, L>* s = new Node<T, L>(t, false);
                s->children[0] = root;
                s->splitChild(0, root);
                int i = 0;
                if (s->keys[0] < key) {
                    i++;
                }
                s->children[i]->insertNonFull(key, value, machineFolder);
                root = s;
            }
            else {
                root->insertNonFull(key, value, machineFolder);
            }
        }
    }
    void traverse() {
        if (root != NULL) {
            root->traverse();
        }
    }
    Node<T, L>* search(T key) {
        if (root == NULL) {
            return NULL;
        }
        return root->search(key);
    }
    void remove(T key, string machineFolder, string& res) {
        if (root == NULL) {
            cout << "Tree is empty" << endl;
            return;
        }
        root->remove(key, machineFolder, res);
        if (root->n == 0) {
            Node<T, L>* temp = root;
            if (root->leaf) {
                root = NULL;
            }
            else {
                root = root->children[0];
            }
            delete(temp);
        }
    }
    void printTree() {
        if (root != NULL) {
            root->printTree();
        }
    }
    string searchvalue(T key) {
        if (root == NULL) {
            return "Tree is empty";
        }
        return root->searchvalue(key, "");
    }
    T* levelorder(T kx, int& cx) {
        T* rk = new T[1000];
        cx = 0;
        if (root == NULL) {
            return rk;
        }
        queue<Node<T, L>*> q;
        q.push(root);
        while (!q.empty()) {
            int n = q.size();
            while (n > 0) {
                Node<T, L>* temp = q.front();
                q.pop();

                for (int i = 0; i < temp->n; i++) {
                    if (temp->keys[i] <= kx) {
                        rk[cx] = temp->keys[i];
                        cx++;
                    }
                }

                if (!temp->leaf) {
                    for (int i = 0; i <= temp->n; i++) {
                        q.push(temp->children[i]);
                    }
                }
                n--;
            }
        }
        return rk;
    }
    T* levelorder1(int& cx) {
        T* rk = new T[1000];
        cx = 0;
        if (root == NULL) {
            return rk;
        }
        queue<Node<T, L>*> q;
        q.push(root);
        while (!q.empty()) {
            int n = q.size();
            while (n > 0) {
                Node<T, L>* temp = q.front();
                q.pop();

                for (int i = 0; i < temp->n; i++) {
                    rk[cx] = temp->keys[i];
                    cx++;
                }

                if (!temp->leaf) {
                    for (int i = 0; i <= temp->n; i++) {
                        q.push(temp->children[i]);
                    }
                }
                n--;
            }
        }
        return rk;
    }

};
void copyFileToMachineFolder(const std::string& sourceFilePath, const std::string& machineFolder) {

    std::filesystem::path sourcePath(sourceFilePath);
    std::string fileName = sourcePath.filename().string();
    std::string destinationFilePath = machineFolder + "/" + fileName;

    std::ifstream sourceFile(sourceFilePath, std::ios::binary);

    if (!sourceFile) {
        std::cerr << "Error opening source file: " << sourceFilePath << std::endl;
        return;
    }

    std::ofstream destinationFile(destinationFilePath, std::ios::binary);

    if (!destinationFile) {
        std::cerr << "Error opening destination file: " << destinationFilePath << std::endl;
        return;
    }

    destinationFile << sourceFile.rdbuf();

    sourceFile.close();
    destinationFile.close();

    std::cout << "File copied to machine folder successfully!" << std::endl;
}
class RoutingNode {
    // Define structure to hold routing information
    // e.g., pointers to other machines or nodes
    Node1* machine;
    // Consider storing IP addresses, ports, or other identifiers
};
class Node1;
class RoutingTable {
public:
    Node1** table; // Table structure to hold routing information
    int size;
    RoutingTable(int size) {
        this->size = size;
        this->table = new Node1 * [size];
        for (int i = 0; i < size; i++) {
            table[i] = NULL;
        }
    }
    RoutingTable() {
        size = 0;
        table = nullptr;
    }

    void updateRoutingTable(int number, Node1* toinsert) {
        // Implement logic to update the routing table
        // This may involve adding or removing entries
        table[number] = toinsert;
    }
};
class Node1 {
public:
    int id;
    Btree<int, string>* b;
    Node1* next;
    string machinepath;
    RoutingTable* rt;
    bool lastnode = false;
    int rtsize;
    Node1(int id1 = 0, int rsize = 0) {
        id = id1;
        next = nullptr;
        rtsize = rsize;
        rt = new RoutingTable(rsize);
    }
    int getNumber(int here) {
        int thiss = pow(2, here);
        thiss += id;
        return thiss;
    }
    void print() {
        for (int i = 0; i < rtsize; i++) {
            if (rt->table[i] == nullptr)
                continue;
            cout << rt->table[i]->id << " ";
        }
        cout << endl;
    }
    Node1* findMachineForKey(int id) {

        if (rt->table[0]->id > id)
            return nullptr;
        for (int i = 0; i < rt->size; i++) {
            if (rt->table[i]->id > id) {
                if (i == 0) {

                    return rt->table[i];
                }
                return rt->table[i - 1];
            }
            if (rt->table[i]->id == id) {
                return rt->table[i];
            }
        }
    }



    //Masla idhar hy
    Node1* findMachineForKey1(int id) {
        //if (current->next == head && (current->id < key || key<head->id)) {
        if (rt->table[0]->id >= id && this->id < id)
            return rt->table[0];
        for (int i = 0; i < rt->size; i++) {
            if (rt->table[i]->id >= id && this->id < id) {
                return rt->table[i - 1];
            }
            if (rt->table[i]->lastnode == 1 && (id > rt->table[i]->id || rt->table[i]->next->id > id)) {
                return rt->table[i]->next;
            }
            /*if (rt->table[i]== && id > rt->table[i]->id) {
                return rt->table[i]->next;
            }*/
        }
        if (rt->table[rt->size - 2]->id == id)
            return rt->table[rt->size - 2];
        else
            return rt->table[rt->size - 1];
    }

};
class RDHT {
public:
    Node1* head;
    Node1* last;
    int count = 0;
    int identifierspace = 0;
    RDHT(int ident) {
        head = nullptr;
        identifierspace = ident;
    }
    void insertmachine(int key, int order) {
        Node1* added = new Node1(key, identifierspace);
        added->machinepath = to_string(key);
        added->b = new Btree<int, string>(order);
        std::filesystem::path dirPath(to_string(key));
        if (!std::filesystem::exists(dirPath)) {
            std::filesystem::create_directory(dirPath);
        }
        if (head == nullptr) {
            head = added;
            added->next = head;
            count++;
        }
        int x = succ(key);
        if (head != nullptr && count > 0) {         //idhr na find krta to 
            Node1* current = head;
            Node1* prev = nullptr;
            do {
                if (current->id >= key)
                    break;
                prev = current;
                current = current->next;
            } while (current->next != head);

            if (prev == nullptr) {  // Insertion at the beginning
                // Find the last Node1
                Node1* last = head;
                while (last->next != head) {
                    last = last->next;
                }
                // Update last Node1's next to new Node1
                last->next = added;
                added->next = head;
                head = added;
            }
            else if (current->next == head && current->id < key) {
                added->next = head;
                current->next = added;
            }
            else {
                if (current->id < key) {
                    added->next = current->next;
                    current->next = added;
                }
                else {
                    prev->next = added;
                    added->next = current;
                }
            }
            Node1* temp = head;
            do {
                temp->lastnode = false; // Set lastnode to false for all nodes
                temp = temp->next;
            } while (temp->next != head); // Continue until the node before the head is reached

            temp->lastnode = true;
            last = temp;
            UpdateRT();
            count++;
        }

        Node1* temp = search(x);
        int count1 = 0;
        int* m = temp->b->levelorder(key, count1);
        string res;
        for (int i = 0; i < count1; i++) {
            temp->b->remove(m[i], temp->machinepath, res);
            added->b->insert(m[i], res, added->machinepath);
        }
    }
    void UpdateRT() {
        Node1* current = head;
        int totalsize = pow(2, identifierspace);
        do {
            for (int i = 0; i < current->rtsize; i++) {
                int number = current->getNumber(i);
                number %= totalsize;
                Node1* to_insert = successor(number);
                current->rt->updateRoutingTable(i, to_insert);
            }
            current = current->next;
        } while (current != head);
    }
    Node1* successor(int key) {
        Node1* current = head;
        do {
            if (current->id >= key)
                return current;
            current = current->next;
        } while (current != head);
        return head;
    }
    Node1* search(int key) {
        Node1* current = head;
        do {
            if (current->id == key)
                return current;
            current = current->next;
        } while (current != head);
        return nullptr;
    }
    Node1* searchmachine(int id) {
        Node1* current = head;
        if (current->id == id) {
            return current;
        }
        while (current->id != id) {
            current = current->findMachineForKey(id);
            if (current == nullptr) {
                return nullptr;
            }
        }
        return current;
    }
    Node1* searchfile(int id, Node1* here) {

        Node1* current = here;
        if (current->id == id) {
            return current;
        }

        while (current->id != id) {
            current = current->findMachineForKey1(id);
            if (current == nullptr) {
                return nullptr;
            }
        }

        return current;
    }
    Node1* searchM(int key, int machine) {
        if (head->id >= key) {
            return head;
        }
        Node1* current = search(machine);
        Node1* nextMachine = nullptr;
        if (key < current->id) {
            if (successor(key) == current) {
                return current;
            }
            Node1* curr = head;
            while (curr != nullptr) {
                nextMachine = curr->findMachineForKey1(key);
                if (nextMachine == nullptr || nextMachine->id >= key) {
                    // If the next machine is null or same as current, it means current machine is responsible for the key
                    return nextMachine;
                }
                if (nextMachine == head && last->id < key) {
                    return nextMachine;
                }
                curr = nextMachine;
            }
        }
        else
        {

            if (current->id == key)
                return current;
            if (current->next == head && (current->id < key || key < head->id)) {
                return head;
            }
            if (!current) {
                cout << "Starting machine with ID " << machine << " not found." << endl;
                return nullptr;
            }
            while (current != nullptr) {
                nextMachine = current->findMachineForKey1(key);
                if (nextMachine == nullptr || nextMachine->id >= key) {
                    // If the next machine is null or same as current, it means current machine is responsible for the key
                    return nextMachine;
                }
                if (nextMachine == head && last->id < key) {
                    return nextMachine;
                }
                current = nextMachine;
            }
        }
        cout << "Machine responsible for key " << key << " not found." << endl;
        return nullptr;
    }


    //Node1 * searchM(int key, int machine) {

    //    Node1* here;

    //    if (machine == head->id) {

    //        here = head;
    //    }

    //    else {

    //        here = searchmachine(machine);

    //    }
    //    cout << here->id;
    //    Node1* what = searchfile(key, here);

    //    return what;

    //}

    int succ(int key) {
        Node1* current = head;
        do {
            if (current->id >= key)
                return current->id;
            current = current->next;
        } while (current != head);
        return head->id;
    }
    int succ1(int key) {
        Node1* current = head;
        do {
            if (current->id >= key)
                return current->id;
            current = current->next;
        } while (current != head);
        return head->id;
    }
    void deletemachine(int key) {
        Node1* current = head;
        Node1* prev = nullptr;
        int x = succ(key + 1);
        Node1* arzi = search(x);
        Node1* tobedeleted = search(key);
        if (tobedeleted == nullptr)
            return;
        int count1 = 0;
        string res = "";
        int* m = tobedeleted->b->levelorder1(count1);
        for (int i = 0; i < count1; i++) {
            tobedeleted->b->remove(m[i], tobedeleted->machinepath, res);
            arzi->b->insert(m[i], res, arzi->machinepath);
        }
        do {
            if (current->id == key)
                break;
            prev = current;
            current = current->next;
        } while (current->next != head);
        if (prev == nullptr) {
            Node1* last = head;
            while (last->next != head) {
                last = last->next;
            }
            Node1* temp = head;
            if (head->next == head) {                                       //If theres only one Node1 inlist
                head = nullptr;
            }
            else {                                                          //If there are multiple Node1s inlist and we are deleting first
                head = head->next;
                last->next = head;
            }
            std::filesystem::path dirPath(current->machinepath);
            if (std::filesystem::exists(dirPath)) {
                std::filesystem::remove_all(dirPath);
            }
            delete temp;
        }
        else if (current->next == head && current->id == key) {
            prev->next = head;
            std::filesystem::path dirPath(current->machinepath);
            if (std::filesystem::exists(dirPath)) {
                std::filesystem::remove_all(dirPath);
            }
            delete current;
        }
        else if (current->next == head && current->id != key)
            return;                                                         //Node1 not found
        else {
            prev->next = current->next;
            std::filesystem::path dirPath(current->machinepath);
            if (std::filesystem::exists(dirPath)) {
                std::filesystem::remove_all(dirPath);
            }
            delete current;
        }
        if (head != nullptr) {
            Node1* temp = head;
            do {
                temp->lastnode = false; // Set lastnode to false for all nodes
                temp = temp->next;
            } while (temp->next != head); // Continue until the node before the head is reached

            temp->lastnode = true;
            last = temp;
        }
        UpdateRT();
    }
    //void insertFile(string machine, string key, string content) {
    //    string mth = succ(key);
    //    Node1* current =search(machine);
    //    while (current->id != mth && current->next !=search(machine)) {
    //        current = current->next;
    //    }
    //    current->b->insert(content);
    //}
    void insertFile(int machine, int key, string content) {

        Node1* ptr = searchM(key, machine);

        if (ptr == nullptr) {
            cout << "Machine with ID " << machine << " not found." << endl;
            return;
        }
            ptr->b->insert(key, content, ptr->machinepath);
    }
    string deleteFile(int key) {
        Node1* ptr = searchM(key, head->id);
        string res = "";
        ptr->b->remove(key, ptr->machinepath, res);
        return res;
    }
    void printlist() {
        if (head == nullptr) return;

        Node1* current = head;
        do {
            cout << "Machine ID : " << current->id << " ";
            current = current->next;
        } while (current != head);
        cout << endl;
    }
    void printrts() {
        Node1* current = head;
        do {
            cout << "Routing Table of Machine ID " << current->id << " is :";
            current->print();
            current = current->next;
        } while (current != head);
    }
};

int main() {
    cout << "Enter Order \n";
    int order;
    order = 5;
    int ident;
    cin >> ident;
    RDHT rdht(ident);
    rdht.identifierspace = ident;
    string fsuc;
    //ask user of identifier space
    // Ask user how many machines he want to add
        //ask if he wants to mannually set id of any machine
       // otherwise ask name of machines hash it and call
        //a.insertmachine(hashedname)
        //before adding check if a.count > 2^identifierspace
    rdht.insertmachine(1, order);
    rdht.insertmachine(4, order);
    rdht.insertmachine(9, order); // Larger than any existing ID
    rdht.printlist(); // Expected output: B010, B020, B030
    //cout << "Enter a string you want to find successor of : " << endl;
    //cin >> fsuc;
    //cout << "Successor of " << fsuc << " is : " << rdht.succ(fsuc)<<endl;
    //cout << "Enter a string you want to find successor of : " << endl;
    //cin >> fsuc;
    //cout << "Successor of " << fsuc << " is : " << rdht.succ(fsuc) << endl;
    //cout << "Enter a string you want to find successor of : " << endl;
    //cin >> fsuc;
    //cout << "Successor of " << fsuc << " is : " << rdht.succ(fsuc) << endl;
    //rdht.insertmachine("B015");
    //rdht.printlist();
    //cout << "Enter a string you want to find successor of : " << endl;
    //cin >> fsuc;
    //cout << "Successor of " << fsuc << " is : " << rdht.succ(fsuc) << endl;
    //rdht.deletemachine("B010");
    //rdht.printlist();
    //cout << endl << endl;
    //cout << "Enter a string you want to find successor of : " << endl;
    //cin >> fsuc;
    //cout << "Successor of " << fsuc << " is : " << rdht.succ(fsuc) << endl;
    //cout << "Enter a string you want to find successor of : " << endl;
    //cin >> fsuc;
    //cout << "Successor of " << fsuc << " is : " << rdht.succ(fsuc) << endl;
    //rdht.insertFile(1, 16, "xyz.jpg");
    //rdht.insertFile(1, 14, "xyz2.txt");
    //rdht.insertFile(3, 19, "xyz3.txt");
    //rdht.insertFile(9, 11, "xyz4.txt");
    rdht.printlist();
    rdht.printrts();
    cout << endl << endl << endl;
    rdht.printrts();
    rdht.insertmachine(20, order);
    rdht.printlist();
    rdht.printrts();
    rdht.insertmachine(11, order);
    rdht.printlist();
    rdht.printrts();
    rdht.insertmachine(28, order);
    rdht.printlist();
    rdht.printrts();
    rdht.insertmachine(18, order);
    rdht.printlist();
    rdht.printrts();
    rdht.insertmachine(21, order);
    rdht.printlist();
    rdht.printrts();
    rdht.insertmachine(14, order);
    rdht.printlist();
    rdht.printrts();
    cout << endl << endl << endl << endl;
    Node1* mp = rdht.searchM(26, 14);
    cout << mp->id;
    rdht.insertFile(14, 26, "xyz.jpg");
    rdht.insertFile(14, 4, "xyz2.txt");
    rdht.insertFile(21, 29, "xyz3.txt");
    rdht.insertFile(28, 31, "xyz4.txt");
    rdht.deleteFile(4);

    //Node1* xd;
    //int arr[9] = { 1,4,9,11,14,18,20,21,28 };
    //for (int i = 0; i < 9; i++) {
    //    cout << arr[i] << "Will access: ";
    //    for (int j = 0; j <= 31; j++) {
    //        if (j % 4 == 0)
    //            cout << "\n";
    //        xd = rdht.searchM(j, arr[i]);
    //        cout << j << " " << xd->id << "\t";
    //    }
    //    cout << endl;
    //}
}