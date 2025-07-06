#include <iostream>
#include "model.h"
using namespace std;

int main() {
    VendingMachine vm;
    vm.loadProducts("input.txt");

    cout << "Choose : \n(1) Buyer \n(2) Owner\n";
    int choice;
    cin >> choice;

    if (choice == 1) {
        Buyer buyer;
        buyer.shop(vm);
    } else if (choice == 2) {
        Owner owner;
        owner.manage(vm);
    } else {
        cout << "Invalid choice. Exiting...\n";
    }

    vm.saveProducts("input.txt");

    return 0;
}
