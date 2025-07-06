#pragma once
#include <unordered_map>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <regex>
#include <cctype>
using namespace std;

class Product {
    int id;
    string name;
    int price;
    int stock;

public:
    Product() : id(0), name(""), price(0), stock(0) {}
    Product(int id, string name, int price, int stock) : id(id), name(name), price(price), stock(stock) {}

    string display() const {
        return to_string(id) + " -- " + name + "  Rs." + to_string(price) + "   Stock - " + to_string(stock) + "\n";
    }

    int getStock() const { return stock; }
    void reduceStock(int qty) { stock -= qty; }
    void addStock(int qty) { stock += qty; }

    int getId() const { return id; }
    int getPrice() const { return price; }
    string getName() const { return name; }

    void setStock(int newStock) { stock = newStock; }
};

class Item {
    Product product;
    int quantity;

public:
    Item() : product(), quantity(0) {}
    Item(Product product, int quantity) : product(product), quantity(quantity) {}

    int getPrice() const { return quantity * product.getPrice(); }

    string getInfo() const {
        return " " + product.getName() + " x" + to_string(quantity) + "     Rs. " + to_string(getPrice()) + "\n";
    }

    void incrementQuantity() { quantity++; }

    const Product& getProduct() const { return product; }
    int getQuantity() const { return quantity; }
};

class Cart {
    unordered_map<int, Item> items;

public:
    void addProduct(const Product &product) {
        int id = product.getId();
        if (!items.count(id)) {
            items[id] = Item(product, 1);
        } else {
            items[id].incrementQuantity();
        }
    }

    string view() const {
        if (items.empty()) return "Cart is empty!\n";
        string s;
        int total = 0;
        for (const auto &it : items) {
            s += it.second.getInfo();
            total += it.second.getPrice();
        }
        s += " Total Price = Rs. " + to_string(total) + "\n";
        return s;
    }

    int getTotal() const {
        int total = 0;
        for (const auto &it : items) total += it.second.getPrice();
        return total;
    }

    unordered_map<int,int> getQuantities() const {
        unordered_map<int, int> result;
        for (const auto &it : items) result[it.first] = it.second.getQuantity();
        return result;
    }

    bool isEmpty() const { return items.empty(); }
};

class VendingMachine {
    vector<Product> products;

public:
    void loadProducts(const string &filename) {
        products.clear();
        ifstream infile(filename);
        if (!infile) return;
        string line;
        while (getline(infile, line)) {
            stringstream ss(line);
            int id, price, stock;
            string name;
            ss >> id >> name >> price >> stock;
            products.emplace_back(id, name, price, stock);
        }
        infile.close();
    }

    void saveProducts(const string &filename) const {
        ofstream outfile(filename);
        for (size_t i = 0; i < products.size(); ++i) {
            outfile << (i + 1) << " " << products[i].getName() << " " << products[i].getPrice() << " " << products[i].getStock() <<"\n";
        }
        outfile.close();
    }

    void displayProducts(bool showOutOfStock = false) const {
        for (const auto &p : products) {
            if (showOutOfStock || p.getStock() > 0)
                cout << p.display();
        }
    }

    Product* selectProduct() {
        cout << " Available Products : \n";
        displayProducts();
        cout << " Select the product ID: ";
        int id;
        cin >> id;
        if (id <= 0 || id > static_cast<int>(products.size())) {
            cout << " Product not found!\n";
            return nullptr;
        }
        return &products[id - 1];
    }

    void reduceStock(int productId, int qty) {
        if (productId <= 0 || productId > static_cast<int>(products.size())) return;
        products[productId - 1].reduceStock(qty);
    }

    void addNewProduct() {
        string name;
        int price, stock;
        cout << " Enter product name: ";
        cin >> name;
        cout << " Enter price: ";
        cin >> price;
        cout << " Enter stock: ";
        cin >> stock;

        int id= products.size() + 1;
        products.emplace_back(id, name, price, stock);
        cout << "New product added.\n";
        saveProducts("input.txt");
    }

    void updateStock() {
        cout << " Enter product ID: ";
        int id, stock;
        cin >> id;
        cout << " Enter additional stock: ";
        cin >> stock;
        if (id > 0 && id <= static_cast<int>(products.size())) {
            products[id - 1].addStock(stock);
            cout << "Stock updated.\n";
            saveProducts("input.txt");
        } else {
            cout << "Invalid Product ID!\n";
        }
    }

    void saveToFile() {
        saveProducts("input.txt");
    }
};

class Buyer {
public:
    void shop(VendingMachine &vm) {
        Cart cart;
        while (true) {
            cout << "(1) Add Item\n(2) View Cart\n(3) Checkout\n";
            int choice;
            cin >> choice;

            if (choice == 1) {
                Product* product = vm.selectProduct();
                if (product && product->getStock() > 0) {
                    cart.addProduct(*product);
                } else {
                    cout << "Product not available.\n";
                }
            } else if (choice == 2) {
                cout << cart.view();
            } else if (choice == 3) {
                if (checkout(cart, vm)) break;
            }
        }
    }

private:
    bool checkout(Cart &cart, VendingMachine &vm) {
        if (cart.isEmpty()) return false;

        cout << " Please pay a total of Rs. " << cart.getTotal() << endl;
        int cash;
        cin >> cash;

        if (cash >= cart.getTotal()) {
            for (const auto &item : cart.getQuantities()) {
                vm.reduceStock(item.first, item.second);
            }
            vm.saveToFile();

            ofstream outfile("profit.txt", ios::app);
            outfile << cart.view() << "\n";
            outfile.close();

            cout << " Change -- Rs. " << cash - cart.getTotal() << "\nThank you for shopping :)\n";
            return true;
        } else {
            cout << " Insufficient Cash\n";
            return false;
        }
    }
};

class Owner {
    const string password = "vending123";

public:
    void manage(VendingMachine &vm) {
        cout << " Enter password: ";
        string input;
        cin >> input;
        if (input != password) {
            cout << "Wrong password!\n";
            return;
        }

        while (true) {
            cout << "(1) Add Product\n(2) Update Stock\n(3) See Profit\n(4) Done\n";
            int choice;
            cin >> choice;

            if (choice == 1) vm.addNewProduct();
            else if (choice == 2) vm.updateStock();
            else if (choice == 3) seeProfit();
            else break;
        }
    }

private:
    void seeProfit() {
        ifstream file("profit.txt");
        string line;
        int total = 0;

        while (getline(file, line)) {
            if (line.find("Total Price") != string::npos) {
                size_t pos = line.find("Rs.");
                if (pos != string::npos) {
                    string numberPart = line.substr(pos + 3);
                    numberPart.erase(remove_if(numberPart.begin(), numberPart.end(), [](char c) { return !isdigit(c); }), numberPart.end());
                    if (!numberPart.empty()) {
                        total += stoi(numberPart);
                    }
                }
            }
        }

        cout << "Total sales is Rs. " << total << endl;
    }
};
