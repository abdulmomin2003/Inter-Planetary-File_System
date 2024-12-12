//
// pch.h
//
#include "cryptopp/sha.h"
#include "cryptopp/filters.h"
#include "cryptopp/hex.h"
#include "cryptopp/files.h"
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<filesystem>
#include <cstdio>
#include<queue>
#include<math.h>
#include "BigInt.h"
#include<thread>
#include<chrono>
using namespace std;
template<class T>
class Node1;
template<class T>
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
    void remove(T key, const string& machineFolder, string& res) {
        int i = 0;
        const int MAX_VALUES = 100; // Adjust the size based on your requirements
        L deletedValues[MAX_VALUES];
        int deletedCount = 0;

        // Find and store the values corresponding to the key being deleted
        while (i < n && keys[i] < key) {
            i++;
        }
        while (i < n && keys[i] == key) {
            deletedValues[deletedCount++] = values[i];
            i++;
        }

        if (leaf) {
            int k = 0;
            for (int j = 0; j < n; j++) {
                if (keys[j] == key) {
                    // Delete the corresponding file from the machine folder
                    string deletedFilePath = machineFolder + "/" + values[j];
                    deleteFile(deletedFilePath);
                }
                else {
                    keys[k] = keys[j];
                    values[k] = values[j];
                    k++;
                }
            }
            n = k;
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

        // Delete all instances of the value from the B-tree
        for (int d = 0; d < deletedCount; ++d) {
            res = deletedValues[d]; // Return the last deleted value
            filesystem::path path(deletedValues[d]);
            string deletedValue = path.filename().string();

            for (int j = 0; j < n; j++) {
                if (values[j] == deletedValue) {
                    // Delete the corresponding file from the machine folder
                    string deletedFilePath = machineFolder + "/" + deletedValue;
                    deleteFile(deletedFilePath);

                    int k = 0;
                    for (int m = 0; m < n; m++) {
                        if (m != j) {
                            keys[k] = keys[m];
                            values[k] = values[m];
                            k++;
                        }
                    }
                    n = k;
                    j--; // Adjust the loop variable to recheck the current index
                }
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
        cout << "]";
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
    void levelorderPrint() {
        if (root == NULL) {
            return;
        }
        queue<Node<T, L>*> q;
        q.push(root);
        while (!q.empty()) {
            int n = q.size();
            cout << "[";
            while (n > 0) {
                Node<T, L>* temp = q.front();
                q.pop();
                temp->print();

                if (!temp->leaf) {
                    for (int i = 0; i <= temp->n; i++) {
                        q.push(temp->children[i]);
                    }
                }
                n--;
            }

            cout << endl;
        }
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
template <class T>
class RoutingNode {
    // Define structure to hold routing information
    // e.g., pointers to other machines or nodes
    Node1<T>* machine;
    // Consider storing IP addresses, ports, or other identifiers
};
template <class T>
class Node1;
template <class T>
class RoutingTable {
public:
    Node1<T>** table; // Table structure to hold routing information
    int size;
    RoutingTable(int size) {
        this->size = size;
        this->table = new Node1<T> *[size];
        for (int i = 0; i < size; i++) {
            table[i] = NULL;
        }
    }
    RoutingTable() {
        size = 0;
        table = nullptr;
    }

    void updateRoutingTable(int number, Node1<T>* toinsert) {
        // Implement logic to update the routing table
        // This may involve adding or removing entries
        table[number] = toinsert;
    }
};
template<class T>
class Node1 {
public:
    T id;
    Btree<T, string>* b;
    Node1<T>* next;
    string machinepath;
    RoutingTable<T>* rt;
    bool lastnode = false;
    int rtsize;
    Node1(T id1 = 0, int rsize = 0) {
        id = id1;
        next = nullptr;
        rtsize = rsize;
        rt = new RoutingTable<T>(rsize);
    }
    T getNumber(T here) {

        T thiss = power11(2, here);
        thiss = thiss + id;
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
    Node1<T>* findMachineForKey(T id) {

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




    Node1<T>* findMachineForKey1(T id) {
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

        for (int i = rt->size - 1; i >= 0; i--) {
            if (rt->table[i]->id > this->id) {
                return rt->table[i];
            }
        }



        if (rt->table[rt->size - 2]->id == id)
            return rt->table[rt->size - 2];
        else if (rt->table[rt->size - 1]->id == id)
            return rt->table[rt->size - 1];
    }
};
template <class T>
class RDHT {
public:
    Node1<T>* head;
    Node1<T>* last;
    int count = 0;
    int identifierspace = 0;

    RDHT(int ident) {
        head = nullptr;
        identifierspace = ident;

    }
    string deleteFile(T key,T id) {
        Node1<T>* ptr = searchM(key, id);
        string res = "";
        ptr->b->remove(key, ptr->machinepath, res);
        return res;
    }
    void insertFile(T machine, T key, string content) {

        Node1<T>* ptr = searchM(key, machine);

        if (ptr == nullptr) {
            cout << "Machine with ID " << machine << " not found." << endl;
            return;
        }
        ptr->b->insert(key, content, ptr->machinepath);
    }
    void insertmachine(T key, int order) {
        Node1<T>* added = new Node1<T>(key, identifierspace);
        string keyyy = key.toString();
        added->machinepath = keyyy;
        added->b = new Btree<T, string>(order);


        std::filesystem::path dirPath((keyyy));
        if (!std::filesystem::exists(dirPath)) {
            std::filesystem::create_directory(dirPath);
        }
        if (head == nullptr) {
            head = added;
            added->next = head;
            count++;
        }
        T x = succ(key);
        if (head != nullptr && count > 0) {         //idhr na find krta to 
            Node1<T>* current = head;
            Node1<T>* prev = nullptr;
            do {
                if (current->id >= key)
                    break;
                prev = current;
                current = current->next;
            } while (current->next != head);

            if (prev == nullptr) {  // Insertion at the beginning
                // Find the last Node1
                Node1<T>* last = head;
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
            Node1<T>* temp = head;
            do {
                temp->lastnode = false; // Set lastnode to false for all nodes
                temp = temp->next;
            } while (temp->next != head); // Continue until the node before the head is reached

            temp->lastnode = true;
            last = temp;
            UpdateRT();
            count++;
        }
        Node1<T>* temp = search(x);
        int count1 = 0;
        T* m = temp->b->levelorder(key, count1);
        string res;
        for (int i = 0; i < count1; i++) {
            temp->b->remove(m[i], temp->machinepath, res);
            added->b->insert(m[i], res, added->machinepath);
        }
    }
    void UpdateRT() {
        Node1<T>* current = head;
        T totalsize = power11(2, identifierspace);
        do {
            for (int i = 0; i < current->rtsize; i++) {
                T number = current->getNumber(i);
                number = number % totalsize;
                Node1<T>* to_insert = successor(number);
                current->rt->updateRoutingTable(i, to_insert);
            }
            current = current->next;
        } while (current != head);
    }
    Node1<T>* successor(T key) {
        Node1<T>* current = head;
        do {
            if (current->id >= key)
                return current;
            current = current->next;
        } while (current != head);
        return head;
    }
    Node1<T>* search(T key) {
        Node1<T>* current = head;
        do {
            if (current->id == key)
                return current;
            current = current->next;
        } while (current != head);
        return nullptr;
    }
    Node1<T>* searchmachine(T id) {
        Node1<T>* current = head;
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
    Node1<T>* searchfile(T id, Node1<T>* here) {

        Node1<T>* current = here;
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
    Node1<T>* searchM(T key, T machine) {
        if (head->id >= key) {
            return head;
        }
        Node1<T>* current = search(machine);
        Node1<T>* nextMachine = nullptr;
        if (key < current->id) {
            if (successor(key) == current) {
                return current;
            }
            Node1<T>* curr = head;
            while (curr != nullptr) {
                nextMachine = curr->findMachineForKey1(key);
                if (nextMachine == nullptr || nextMachine->id >= key) {
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
            {
                return current;
            }
            if (current->next == head && (current->id < key || key < head->id)) {
                return head;
            }
            if (!current) {
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
        return nullptr;
    }

    T succ(T key) {
        Node1<T>* current = head;
        do {
            if (current->id >= key)
                return current->id;
            current = current->next;
        } while (current != head);
        return head->id;
    }
    T succ1(T key) {
        Node1<T>* current = head;
        do {
            if (current->id >= key)
                return current->id;
            current = current->next;
        } while (current != head);
        return head->id;
    }
    void deletemachine(T key) {
        Node1<T>* current = head;
        Node1<T>* prev = nullptr;
        T x = succ(key + 1);
        Node1<T>* arzi = search(x);
        Node1<T>* tobedeleted = search(key);
        if (tobedeleted == nullptr)
            return;
        int count1 = 0;
        string res = "";
        T* m = tobedeleted->b->levelorder1(count1);
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
            Node1<T>* last = head;
            while (last->next != head) {
                last = last->next;
            }
            Node1<T>* temp = head;
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
            return;                                                    //Node1 not found
        else {
            prev->next = current->next;
            std::filesystem::path dirPath(current->machinepath);
            if (std::filesystem::exists(dirPath)) {
                std::filesystem::remove_all(dirPath);
            }
            delete current;
        }
        if (head != nullptr) {
            Node1<T>* temp = head;
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

    void printlist() {
        if (head == nullptr) return;

        Node1<T>* current = head;
        do {
            cout << "Machine ID : " << current->id << " ";
            current = current->next;
        } while (current != head);
        cout << endl;
    }
    void printrts() {
        Node1<T>* current = head;
        do {
            cout << "Routing Table of Machine ID " << current->id << " is :";
            current->print();
            current = current->next;
        } while (current != head);
    }
};
string shaString(string str,int indent) {
    CryptoPP::SHA1 sha1;
    string hash = "";
    string binaryhash = "";
    char arr[40];
    string contents = str;
    CryptoPP::StringSource(contents, true, new CryptoPP::HashFilter(sha1, new CryptoPP::HexEncoder(new CryptoPP::StringSink(hash))));
    for (int i = 0; i < 40; i++) {
        if (hash[i] == '0') {
            binaryhash += "0000";
        }
        else if (hash[i] == '1') {
            binaryhash += "0001";
        }
        else if (hash[i] == '2') {
            binaryhash += "0010";
        }
        else if (hash[i] == '3') {
            binaryhash += "0011";
        }
        else if (hash[i] == '4') {
            binaryhash += "0100";
        }
        else if (hash[i] == '5') {
            binaryhash += "0101";
        }
        else if (hash[i] == '6') {
            binaryhash += "0110";
        }
        else if (hash[i] == '7') {
            binaryhash += "0111";
        }
        else if (hash[i] == '8') {
            binaryhash += "1000";
        }
        else if (hash[i] == '9') {
            binaryhash += "1001";
        }
        else if (hash[i] == 'A') {
            binaryhash += "1010";
        }
        else if (hash[i] == 'B') {
            binaryhash += "1011";
        }
        else if (hash[i] == 'C') {
            binaryhash += "1100";
        }
        else if (hash[i] == 'D') {
            binaryhash += "1101";
        }
        else if (hash[i] == 'E') {
            binaryhash += "1110";
        }
        else if (hash[i] == 'F') {
            binaryhash += "1111";
        }
    }
    cout <<"Binary of Hashed Key is (160 bits) :" << binaryhash << endl << endl << endl;
    string m = "";
    int j = 0;
    if (indent < 160) {
        for (int i = 160 - indent; i < 160; i++) {
            m += binaryhash[i];
        }
    }
    else {
        m = binaryhash;
    }
    cout << "After Extracting " << indent << " Bits :" << m<<endl;
    BigInt b(m);
    cout << endl << endl;
    m = binaryToDecimal(m);

    cout <<"After Converting These bits to decimal " << m << endl << endl;

    return m;
}
string shaFile(string str, int indent) {
    CryptoPP::SHA1 sha1;
    string hash = "";
    string binaryhash = "";
    char arr[40];
    ifstream file(str, ios::binary);
    if (!file) {
        cerr << "Error: Unable to open file" << endl;
    }
    stringstream buffer;
    buffer << file.rdbuf();
    string contents = buffer.str();
    CryptoPP::StringSource(contents, true, new CryptoPP::HashFilter(sha1, new CryptoPP::HexEncoder(new CryptoPP::StringSink(hash))));
    for (int i = 0; i < 40; i++) {
        if (hash[i] == '0') {
            binaryhash += "0000";
        }
        else if (hash[i] == '1') {
            binaryhash += "0001";
        }
        else if (hash[i] == '2') {
            binaryhash += "0010";
        }
        else if (hash[i] == '3') {
            binaryhash += "0011";
        }
        else if (hash[i] == '4') {
            binaryhash += "0100";
        }
        else if (hash[i] == '5') {
            binaryhash += "0101";
        }
        else if (hash[i] == '6') {
            binaryhash += "0110";
        }
        else if (hash[i] == '7') {
            binaryhash += "0111";
        }
        else if (hash[i] == '8') {
            binaryhash += "1000";
        }
        else if (hash[i] == '9') {
            binaryhash += "1001";
        }
        else if (hash[i] == 'A') {
            binaryhash += "1010";
        }
        else if (hash[i] == 'B') {
            binaryhash += "1011";
        }
        else if (hash[i] == 'C') {
            binaryhash += "1100";
        }
        else if (hash[i] == 'D') {
            binaryhash += "1101";
        }
        else if (hash[i] == 'E') {
            binaryhash += "1110";
        }
        else if (hash[i] == 'F') {
            binaryhash += "1111";
        }
    }
    cout << "Binary of Hashed Key is (160 bits) :" << binaryhash << endl << endl << endl;
    string m = "";
    int j = 0;
    if (indent < 160) {
        for (int i = 160 - indent; i < 160; i++) {
            m += binaryhash[i];
        }
    }
    else {
        m = binaryhash;
    }
    cout << "After Extracting " << indent << " Bits :" << m << endl;
    BigInt b(m);
    cout << endl << endl;
    m = binaryToDecimal(m);

    cout << "After Converting These bits to decimal " << m << endl << endl;

    return m;
}
void displayMenu() {
    int sec = 0;
    this_thread::sleep_for(chrono::seconds(sec)); // Adjust the duration as needed
    std::cout << "\t\t||===== Distributed Hash Table Menu ====||" << std::endl;
    this_thread::sleep_for(chrono::seconds(sec)); // Adjust the duration as needed
    std::cout << "\t\t||1. Insert Machine\t\t\t||" << std::endl;
    this_thread::sleep_for(chrono::seconds(sec)); // Adjust the duration as needed
    std::cout << "\t\t||2. Insert File\t\t\t||" << std::endl;
    this_thread::sleep_for(chrono::seconds(sec)); // Adjust the duration as needed
    std::cout << "\t\t||3. Delete Machine\t\t\t||" << std::endl;
    this_thread::sleep_for(chrono::seconds(sec)); // Adjust the duration as needed
    std::cout << "\t\t||4. Search for File\t\t\t||" << std::endl;
    this_thread::sleep_for(chrono::seconds(sec)); // Adjust the duration as needed
    std::cout << "\t\t||5. Delete File\t\t\t||" << std::endl;
    this_thread::sleep_for(chrono::seconds(sec)); // Adjust the duration as needed
    std::cout << "\t\t||6. Print List of Machines\t\t||" << std::endl;
    this_thread::sleep_for(chrono::seconds(sec)); // Adjust the duration as needed
    std::cout << "\t\t||7. Print Routing Table of a machine\t||" << std::endl;
    this_thread::sleep_for(chrono::seconds(sec)); // Adjust the duration as needed
    std::cout << "\t\t||8. Print B-Tree of a machine\t\t||" << std::endl;
    this_thread::sleep_for(chrono::seconds(sec)); // Adjust the duration as needed
    std::cout << "\t\t||9. Exit\t\t\t\t||" << std::endl;
    this_thread::sleep_for(chrono::seconds(sec)); // Adjust the duration as needed
    std::cout << "\t\t||======================================||" << std::endl;
    this_thread::sleep_for(chrono::seconds(sec)); // Adjust the duration as needed
    std::cout << "\t\t\t\tEnter your choice: ";
}

int main() {
    // Create a DHT with an identifier space of 10
    int identifierspace=0,order=0;
    cout << "Enter Identifier Space : ";
    cin >> identifierspace;
    RDHT<BigInt> dht(identifierspace);
    cout << "Enter Order of B-Trees : ";
    cin >> order;
    int choice;
    do {
        displayMenu();
        std::cin >> choice;
        Node1<BigInt>* mch12;
        string mid;
        switch (choice) {
        case 1: {
            string machinename;
            int choice = -1;
            int numm = 0;
            cout << "Enter Number of Machines you want to enter : ";
            cin >> numm;
            for (int i = 0; i < numm; i++) {
                std::cout << "Enter Machine Name : ";
                std::cin >> machinename;
                cout << "Press 0 if you want to mannually assign ID and 1 if you want to automatically assign id's\n";
                cin >> choice;
                if (choice == 0) {
                    dht.printlist();
                    cout << "Enter Machine Id that doesnot exists and is between range of 2^identifierspace\n";
                    string x;
                    cin >> x;
                    dht.insertmachine(x, order);
                    dht.printlist();
                  //  break;
                }
                else {
                    string m = shaString(machinename, identifierspace);
                    dht.insertmachine(m, order);
                    dht.printlist();
                   // break;
                }
            }
            break;
        }
        case 2: {
            string path, fileKey;
            string mn;
            std::cout << "Enter path of file you want to store ";
            std::cin >> path;
            fileKey = shaFile(path, identifierspace);
            dht.printlist();
            std::cout << "Enter the machine from which you want to insert file from : ";
            cin >> mn;
            Node1<BigInt>*dh=dht.search(mn);
            if (dh == nullptr) {
                cout << "No machine with such id\n";
                break;
            }
            dht.insertFile(mn, fileKey, path);
            break;
        }
        case 3: {
            if (dht.head == nullptr) {
                cout << "There is No machine inserted \n";
                break;
            }
            dht.printlist();
            string machineToDelete;
            std::cout << "Enter Machine ID to delete: ";
            std::cin >> machineToDelete;
            Node1<BigInt>* dh1 = dht.search(machineToDelete);
            if (dh1 == nullptr) {
                cout << "No machine with such id\n";
                break;
            }
            dht.deletemachine(machineToDelete);
            break;
        }
        case 4: {
            string fileKey;
            std::cout << "Enter File Key to search: ";
            std::cin >> fileKey;
            Node1<BigInt>* fileMachine = dht.searchM(fileKey, dht.head->id);
            string xd1 = fileMachine->b->searchvalue(fileKey);
            cout <<"Path of requested file is: " << xd1 << endl;
            break;
        }
        case 5: {
            string fileToDelete,machinetodel;
            std::cout << "Enter File Key to delete: ";
            std::cin >> fileToDelete;
            std::cout << "Enter Machine id you want to delete from : ";
            std::cin >> machinetodel;
            std::string deletedContent = dht.deleteFile(fileToDelete,machinetodel);
            std::cout << "Deleted content: " << deletedContent << std::endl;
            break;
        }
        case 6:
            dht.printlist();
            break;
        case 7:
            dht.printlist();
            std::cout << "Enter a machine you want to print routing table of : ";
            std::cin >> mid;
            mch12=dht.search(mid);
            mch12->print();
            break;
        case 8:
            dht.printlist();
            std::cout << "Enter a machine you want to print B-Tree of : ";
            cin >> mid;
            mch12 = dht.search(mid);
            mch12->b->levelorderPrint();
            break;
        case 9:
            std::cout << "Exiting..." << std::endl;
            break;
        default:
            std::cout << "Invalid choice. Please enter a valid option." << std::endl;
            break;
        }
    } while (choice != 9);

    return 0;
}
