#pragma once
#include <iostream>
#include <string>

using namespace std;

class BigInt
{
	int* val;
	int len;
public:
	BigInt(int val);
	BigInt(int length, int* array);
	BigInt(const string& text);
	BigInt(const BigInt& copy);
	BigInt(const char*);
	void Print();
	BigInt operator + (const BigInt& val) const;
	BigInt operator + (int val) const;
	BigInt operator - (const BigInt& val) const;
	BigInt operator - (int val) const;
	BigInt operator* (const BigInt& val) const;
	//I want mod operator here
	BigInt operator / (const BigInt& val) const;
	BigInt operator / (int val) const;
	BigInt operator % (const BigInt& val) const;
	BigInt operator % (int val) const;
	string toString();

	// Compound Assignment Operators

	BigInt operator += (const BigInt& rhs);
	BigInt operator -= (const BigInt& rhs);
	BigInt operator *= (const BigInt& rhs);

	bool operator == (const BigInt& val) const;
	bool operator != (const BigInt& val) const;
	bool operator < (const BigInt& val) const;
	bool operator < (int val) const;
	bool operator >(int val) const;
	bool operator <= (const BigInt& val) const;
	bool operator > (const BigInt& val) const;
	bool operator >= (const BigInt& val) const;
	bool operator  == (int val) const;
	bool operator  != (int val) const;
	bool operator  <= (int val) const;
	bool operator  >= (int val) const;

	// Unary Operators 
	BigInt& operator++(); // Pre-increment Operator 
	BigInt operator++(int); // Post-increment Operator 
	BigInt& operator--(); // Pre-decrement Operator
	BigInt operator--(int); // Post-decrement Operator
	BigInt& operator=(const BigInt& val); // Assignment Operator
	operator string();

	void removeLeadingZeros(); // removes leading zeros from the BigInt
	// return value of the BigInt as string
	~BigInt(); // destructor
	friend ostream& operator<<(ostream& output, const BigInt& val); // outputs the BigInt 
	friend istream& operator>>(istream& input, BigInt& val); // inputs the BigInt
};
//Make constructor for char* here
BigInt::BigInt(const char val[]) {
	cout << "Hamza" << endl;
	string s = val;
	this->len = s.length();
	this->val = new int[len];
	for (int i = 0; i < len; i++)
	{
		this->val[i] = s[i] - 48;

	}

}
// Add this function to the BigInt class
void BigInt::removeLeadingZeros() {
	int start = 0;
	while (start < len - 1 && val[start] == 0) {
		start++;
	}

	if (start == len - 1 && val[start] == 0) {
		// If the BigInt value is only zeros, set len to 1 and val[0] to 0
		delete[] val;
		val = new int[1] {0};
		len = 1;
	}
	else if (start > 0) {
		int* newArr = new int[len - start];
		for (int i = start; i < len; i++) {
			newArr[i - start] = val[i];
		}
		delete[] val;
		val = newArr;
		len -= start;
	}
}


//BigInt BigInt::operator/(int val) const
//do the division operator here
BigInt BigInt::operator/(int val) const
{
	if (val == 0) {
		throw std::invalid_argument("Division by zero");
	}

	BigInt quotient = *this;

	int carry = 0;
	for (int i = 0; i < quotient.len; i++) {
		int curDigit = quotient.val[i] + carry * 10;
		quotient.val[i] = curDigit / val;
		carry = curDigit % val;
	}

	quotient.removeLeadingZeros();

	return quotient;
}



BigInt BigInt::operator%(int val) const
{
	if (val == 0) {
		throw std::invalid_argument("Modulus by zero");
	}

	BigInt remainder = *this;

	// Perform long division
	while (remainder >= val)
	{
		BigInt multiple = 1;
		BigInt product = val;

		// Find the largest multiple of the divisor less than or equal to the remainder
		while ((product * 10) <= remainder)
		{
			product *= 10;
			multiple *= 10;
		}

		// Subtract the multiple of the divisor from the remainder
		remainder -= product;
	}

	// Remove leading zeros from the remainder
	remainder.removeLeadingZeros();

	return remainder;
}


BigInt BigInt::operator%(const BigInt& val) const
{
	if (val == 0) {
		throw std::invalid_argument("Modulus by zero");
	}

	BigInt remainder = *this;

	// Perform long division
	while (remainder >= val)
	{
		BigInt multiple = 1;
		BigInt product = val;

		// Find the largest multiple of the divisor less than or equal to the remainder
		while ((product * 10) <= remainder)
		{
			product *= 10;
			multiple *= 10;
		}

		// Subtract the multiple of the divisor from the remainder
		remainder -= product;
	}

	// Remove leading zeros from the remainder
	remainder.removeLeadingZeros();


	return remainder;

}

// Modify the division operator as follows:
BigInt BigInt::operator/(const BigInt& val) const
{
	// Check if the divisor is zero
	if (val == 0) {
		throw std::invalid_argument("Division by zero");
	}

	BigInt quotient = 0;
	BigInt remainder = *this; // Copy of the dividend

	// Perform long division
	while (remainder >= val)
	{
		BigInt multiple = 1;
		BigInt product = val;

		// Find the largest multiple of the divisor less than or equal to the remainder
		while ((product * 10) <= remainder)
		{
			product *= 10;
			multiple *= 10;
		}

		// Subtract the multiple of the divisor from the remainder
		remainder -= product;

		// Add the multiple to the quotient
		quotient += multiple;
	}

	// Remove leading zeros from the quotient
	quotient.removeLeadingZeros();

	return quotient;
}


BigInt& BigInt::operator=(const BigInt& val)
{
	if (this == &val)
		return *this;
	delete[] this->val;
	this->len = val.len;
	this->val = new int[len];
	for (int i = 0; i < len; i++)
	{
		this->val[i] = val.val[i];

	}
	return *this;
}
BigInt::BigInt(int val = 0)
{
	this->len = 0;
	int val2 = val;
	while (val > 0) {
		len++;
		val /= 10;
	}

	this->val = new int[len];
	for (int i = len - 1; i >= 0; i--)
	{
		this->val[i] = 0;
	}
	for (int i = len - 1; i >= 0; i--)
	{
		this->val[i] = val2 % 10;
		val2 /= 10;
	}
}

BigInt::BigInt(int length, int* array)
{
	this->len = length;
	val = new int[length];
	for (int i = 0; i < length; i++)
		val[i] = array[i];
}

BigInt::BigInt(const string& text)
{
	this->len = 1;
	while (text[len] != '\0')
		len++;
	this->val = new int[len];
	for (int i = 0; i < len; i++)
	{
		this->val[i] = text[i] - 48;

	}
}

BigInt::BigInt(const BigInt& copy)
{
	this->len = copy.len;
	this->val = new int[len];
	for (int i = 0; i < len; i++)
	{
		this->val[i] = copy.val[i];

	}
}

void BigInt::Print()
{
	for (int i = 0; i < len; i++)
		cout << val[i];
	cout << "\n";
}


BigInt BigInt::operator+(const BigInt& val) const {
	int temp = (this->len >= val.len) ? this->len : (val.len + 1);
	int* arr = new int[temp] { 0 };

	int carry = 0;
	int j = this->len - 1;
	int k = val.len - 1;

	for (int i = temp - 1; i >= 0; i--) {
		int sum = carry;
		if (j >= 0)
			sum += this->val[j];
		if (k >= 0)
			sum += val.val[k];

		arr[i] = sum % 10;
		carry = sum / 10;
		j--;
		k--;
	}

	if (carry != 0) {
		int* newArr = new int[temp + 1];
		newArr[0] = carry;
		for (int i = 1; i <= temp; i++) {
			newArr[i] = arr[i - 1];
		}
		delete[] arr;
		arr = newArr;
		temp += 1;
	}

	int start = 0;
	while (start < temp - 1 && arr[start] == 0) {
		start++;
	}
	if (start == temp - 1 && arr[start] == 0) {
		delete[] arr;
		return BigInt(0);
	}
	return BigInt(temp - start, arr + start);

}

BigInt BigInt::operator+(int val) const
{
	int size = sizeof(val) / 4;
	int temp = (this->len >= size) ? this->len : size + 1;

	int* arr = new int [temp] {0};

	int carry = 0;
	int j = this->len - 1;
	int k = size - 1;

	for (int i = temp - 1; i >= 0; i--) {
		int sum = carry;
		if (j >= 0)
			sum += this->val[j];

		sum += val % 10;
		val /= 10;
		arr[i] = sum % 10;
		carry = sum / 10;
		j--;
		k--;
	}

	if (carry != 0) {
		int* newArr = new int[temp + 1];
		newArr[0] = carry;
		for (int i = 1; i <= temp; i++) {
			newArr[i] = arr[i - 1];
		}
		delete[] arr;
		arr = newArr;
		temp += 1;
	}

	int start = 0;
	while (start < temp - 1 && arr[start] == 0) {
		start++;
	}
	if (start == temp - 1 && arr[start] == 0) {
		delete[] arr;
		return BigInt(0);
	}
	return BigInt(temp - start, arr + start);

}

BigInt BigInt::operator-(int val) const {
	int size = 0;
	int valCopy = val;
	while (valCopy > 0) {
		size++;
		valCopy /= 10;
	}

	int temp = (this->len >= size) ? this->len : size;

	int* arr = new int[temp] {0};

	int borrow = 0;
	int j = this->len - 1;
	int k = size - 1;

	for (int i = temp - 1; i >= 0; i--) {
		int diff = borrow;
		if (j >= 0)
			diff += this->val[j];

		diff -= val % 10;
		val /= 10;

		if (diff < 0) {
			borrow = -1;
			diff += 10;
		}
		else {
			borrow = 0;
		}

		arr[i] = diff;
		j--;
		k--;
	}

	// Remove leading zeros
	int start = 0;
	while (start < temp - 1 && arr[start] == 0) {
		start++;
	}
	if (start == temp - 1 && arr[start] == 0) {
		delete[] arr;
		return BigInt(0);
	}
	return BigInt(temp - start, arr + start);
}

//Do the minus operator here
BigInt BigInt::operator-(const BigInt& val) const
{
	int temp = (this->len >= val.len) ? this->len : val.len;
	int* arr = new int[temp] {0};

	int borrow = 0;
	int j = this->len - 1;
	int k = val.len - 1;

	for (int i = temp - 1; i >= 0; i--) {
		int diff = borrow;
		if (j >= 0)
			diff += this->val[j];
		if (k >= 0)
			diff -= val.val[k];

		if (diff < 0) {
			borrow = -1;
			diff += 10;
		}
		else {
			borrow = 0;
		}

		arr[i] = diff;
		j--;
		k--;
	}

	int start = 0;
	while (start < temp - 1 && arr[start] == 0) {
		start++;
	}

	if (start == temp - 1 && arr[start] == 0) {
		delete[] arr;
		return BigInt(0);
	}
	return BigInt(temp - start, arr + start);
}

BigInt BigInt::operator*(const BigInt& val) const {
	int temp = this->len + val.len;
	int* arr = new int[temp] { 0 };

	for (int i = this->len - 1; i >= 0; i--) {
		int carry = 0;
		for (int j = val.len - 1; j >= 0; j--) {
			int product = (this->val[i] * val.val[j]) + carry + arr[i + j + 1];
			carry = product / 10;
			arr[i + j + 1] = product % 10;
		}
		arr[i] += carry;
	}

	// Find the first non-zero digit in the result
	int start = 0;
	while (start < temp - 1 && arr[start] == 0) {
		start++;
	}

	// If the entire result is zero, return a single zero
	if (start == temp - 1 && arr[start] == 0) {
		delete[] arr;
		return BigInt(0);
	}

	return BigInt(temp - start, arr + start);
}


BigInt BigInt::operator+=(const BigInt& val)
{
	int temp = (this->len >= val.len) ? this->len : (val.len + 1);
	int* arr = new int[temp] { 0 };

	int carry = 0;
	int j = this->len - 1;
	int k = val.len - 1;

	for (int i = temp - 1; i >= 0; i--) {
		int sum = carry;
		if (j >= 0)
			sum += this->val[j];
		if (k >= 0)
			sum += val.val[k];

		arr[i] = sum % 10;
		carry = sum / 10;
		j--;
		k--;
	}

	if (carry != 0) {
		int* newArr = new int[temp + 1];
		newArr[0] = carry;
		for (int i = 1; i <= temp; i++) {
			newArr[i] = arr[i - 1];
		}
		delete[] arr;
		arr = newArr;
		temp += 1;
	}
	this->len = temp;
	this->val = arr;

	removeLeadingZeros();
	return *this;
}

BigInt BigInt::operator-=(const BigInt& val) {
	int temp = (this->len >= val.len) ? this->len : val.len;

	int* arr = new int[temp] {0};

	int borrow = 0;
	int j = this->len - 1;
	int k = val.len - 1;

	for (int i = temp - 1; i >= 0; i--) {
		int diff = borrow;
		if (j >= 0)
			diff += this->val[j];
		if (k >= 0)
			diff -= val.val[k];

		if (diff < 0) {
			borrow = -1;
			diff += 10;
		}
		else {
			borrow = 0;
		}

		arr[i] = diff;
		j--;
		k--;
	}

	// Remove leading zeros
	int start = 0;
	while (start < temp - 1 && arr[start] == 0) {
		start++;
	}

	delete[] this->val;
	this->val = new int[temp - start];
	this->len = temp - start;

	for (int i = 0; i < this->len; i++) {
		this->val[i] = arr[i + start];
	}
	removeLeadingZeros();
	return *this;
}

BigInt BigInt::operator*=(const BigInt& val)
{
	int temp = this->len + val.len;
	int* arr = new int[temp] { 0 };

	for (int i = this->len - 1; i >= 0; i--) {
		int carry = 0;
		for (int j = val.len - 1; j >= 0; j--) {
			int product = (this->val[i] * val.val[j]) + carry + arr[i + j + 1];
			carry = product / 10;
			arr[i + j + 1] = product % 10;
		}
		arr[i] += carry;
	}

	// Remove leading zeros

	this->len = temp;
	this->val = arr;
	removeLeadingZeros();
	return *this;
}
bool BigInt::operator==(int val)const {
	BigInt intAsBigInt(val);
	return (*this) == intAsBigInt;
}
bool BigInt::operator!=(int val)const {
	BigInt intAsBigInt(val);
	return (*this) != intAsBigInt;
}
bool BigInt::operator<=(int val)const {
	BigInt intAsBigInt(val);
	return (*this) <= intAsBigInt;
}
bool BigInt::operator>=(int val)const {
	BigInt intAsBigInt(val);
	return (*this) >= intAsBigInt;
}

bool BigInt::operator==(const BigInt& val) const
{
	if (this->len != val.len)
		return false;

	for (int i = 0; i < this->len; i++) {
		if (this->val[i] != val.val[i])
			return false;
	}

	return true;
}

bool BigInt::operator!=(const BigInt& val) const
{
	if (this->len != val.len)
		return true;
	for (int i = 0; i < this->len; i++) {
		if (this->val[i] == val.val[i])
			return false;
	}
	return true;

}

bool BigInt::operator<(const BigInt& val) const
{
	if (this->len > val.len)
		return false;
	if (this->len < val.len)
		return true;

	for (int i = 0; i < this->len; i++) {
		if (this->val[i] < val.val[i]) {
			return true;
		}
		else if (this->val[i] > val.val[i]) {
			return false;
		}
	}

	return false;
}
//Write this function here
bool BigInt::operator<(int val) const {
	// Create a temporary BigInt object for the integer
	BigInt intAsBigInt(val);

	// Use the existing operator< for BigInt objects
	return (*this) < intAsBigInt;
}
bool BigInt::operator>(int val) const {
	// Create a temporary BigInt object for the integer
	BigInt intAsBigInt(val);

	// Use the existing operator< for BigInt objects
	return (*this) > intAsBigInt;
}


bool BigInt::operator<=(const BigInt& val) const
{
	if (this->len > val.len)
		return false;
	if (this->len < val.len)
		return true;

	for (int i = 0; i < this->len; i++) {
		if (this->val[i] < val.val[i]) {
			return true;
		}
		else if (this->val[i] > val.val[i]) {
			return false;
		}
	}

	return true;
}

bool BigInt::operator>(const BigInt& val) const
{
	if (this->len > val.len)
		return true;
	if (this->len < val.len)
		return false;

	for (int i = 0; i < this->len; i++) {
		if (this->val[i] < val.val[i]) {
			return false;
		}
		else if (this->val[i] > val.val[i]) {
			return true;
		}
	}

	return false;
}

bool BigInt::operator>=(const BigInt& val) const
{
	if (this->len > val.len)
		return true;
	if (this->len < val.len)
		return false;

	for (int i = 0; i < this->len; i++) {
		if (this->val[i] < val.val[i]) {
			return false;
		}
		else if (this->val[i] > val.val[i]) {
			return true;
		}
	}

	return true;
}

BigInt& BigInt::operator++() {
	int carry = 1;
	for (int i = len - 1; i >= 0; i--) {
		int sum = val[i] + carry;
		val[i] = sum % 10;
		carry = sum / 10;
	}

	if (carry > 0) {
		int* newArr = new int[len + 1];
		newArr[0] = carry;
		for (int i = 0; i < len; i++) {
			newArr[i + 1] = val[i];
		}
		delete[] val;
		val = newArr;
		len += 1;
	}
	removeLeadingZeros();
	return *this;
}

BigInt BigInt::operator++(int)
{
	BigInt copy(this->len, this->val);
	int carry = 1;
	for (int i = len - 1; i >= 0; i--) {
		int sum = val[i] + carry;
		val[i] = sum % 10;
		carry = sum / 10;
	}

	if (carry > 0) {
		int* newArr = new int[len + 1];
		newArr[0] = carry;
		for (int i = 0; i < len; i++) {
			newArr[i + 1] = val[i];
		}
		delete[] val;
		val = newArr;
		len += 1;
	}
	removeLeadingZeros();
	return copy;
}

BigInt& BigInt::operator--() {
	int borrow = 1;
	for (int i = len - 1; i >= 0; i--) {
		int diff = val[i] - borrow;
		if (diff < 0) {
			val[i] = 9;
			borrow = 1;
		}
		else {
			val[i] = diff;
			borrow = 0;
			break;
		}
	}
	int start = 0;
	while (start < len - 1 && val[start] == 0) {
		start++;
	}

	if (start == len - 1 && val[start] == 0) {
		val[start] = -1;
	}

	if (start > 0) {
		int* newArr = new int[len - start];
		for (int i = start; i < len; i++) {
			newArr[i - start] = val[i];
		}
		delete[] val;
		val = newArr;
		len -= start;
	}

	return *this;
}

BigInt BigInt::operator--(int)
{
	BigInt copy(this->len, this->val);
	int borrow = 1;
	for (int i = len - 1; i >= 0; i--) {
		int diff = val[i] - borrow;
		if (diff < 0) {
			val[i] = 9;
			borrow = 1;
		}
		else {
			val[i] = diff;
			borrow = 0;
			break;
		}
	}
	int start = 0;
	while (start < len - 1 && val[start] == 0) {
		start++;
	}

	if (start == len - 1 && val[start] == 0) {
		val[start] = -1;
	}

	if (start > 0) {
		int* newArr = new int[len - start];
		for (int i = start; i < len; i++) {
			newArr[i - start] = val[i];
		}
		delete[] val;
		val = newArr;
		len -= start;
	}
	return copy;
}

BigInt::operator string() {
	std::string result;

	if (len == 1 && val[0] == -1) {
		result = "-1";
	}
	else {
		for (int i = 0; i < len; i++) {
			result += to_string(val[i]);
		}
	}

	return result;
}

BigInt::~BigInt()
{
	delete[] val;
}

ostream& operator<<(ostream& output, const BigInt& val)
{
	//	output << "\nThe number is: ";
	for (int i = 0; i < val.len; i++)
		output << val.val[i];
	//output << "\n";
	return output;
}

istream& operator>>(istream& input, BigInt& val)
{
	char a;
	cout << "Enter the number: ";
	for (int i = 0; i < val.len; i++)
	{
		input >> a;
		val.val[i] = a - 48;
	}
	return input;
}

string BigInt::toString() {
	string res;
	for (int i = 0; i < len; i++)
	{
		res += to_string(val[i]);
	}
	return res;
}

string removeLeadingZero(string a) {
	int i = 0;
	while (a[i] == '0') {
		i++;
	}

	// If the entire string is zeros, return a single zero
	if (i == a.size()) {
		return "0";
	}

	return a.substr(i);
}

string power(BigInt a, BigInt b) {
	BigInt t = 1;
	for (int i = 0; i < stoi(b.toString()); i++)
	{
		t *= a;
	}
	return removeLeadingZero(t.toString());
}
BigInt power11(BigInt a, BigInt b) {
	BigInt t = 1;
	for (int i = 0; b > i; i++)
	{
		t *= a;
	}
	t = removeLeadingZero(t.toString());
	return t;
}


string binaryToDecimal(string a) {
	BigInt t = 0;
	for (int i = 0; i < a.size(); i++)
	{
		t *= 2;
		t += a[i] - '0';
	}
	return removeLeadingZero(t.toString());
}

//int main() {
//	BigInt a = "29";
//	BigInt b = "3000000000000";
//	b = a;
//	cout << b << endl;
//	cout << b % a << endl;
//	cout << b << endl;
//	BigInt c = b % a;
//	if (c == "0") {
//		cout << "Yes" << endl;
//	}
//	else {
//		cout << "No" << endl;
//	}
//}