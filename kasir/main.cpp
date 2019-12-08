//
//  main.cpp
//  kasir
//
//  Created by Andre Setiawan A on 01/06/19.
//  Copyright © 2019 Andre Setiawan A. All rights reserved.
//
#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <thread>

using namespace std;


const string lineSeparator = "========================================================";
const string stockFileName = "Stok.dat";

string userName;

//Model Data
struct barang {
    string nama;
    int harga{0};
    int stok{0};
};

//model pembayaran
struct pembayaran {
    int totalHarga;
    int totalBayar;
    int kembalian;
};

//get Curent Date
string getCurentDate() {
    time_t rawTimeData = time(nullptr);
    struct tm timeModel{};
    char buf[80];
    
    timeModel = *localtime(&rawTimeData);
    strftime(buf, sizeof(buf), "%d-%m-%Y", &timeModel);
    
    return buf;
}

//get curent date &time
string getCurentDateTime() {
    time_t rawTimeData = time(nullptr);
    return ctime(&rawTimeData);
}

//Read size of data
int readSizeData(const string &fileName) {
    int size = 0;
    
    try {
        ifstream fileIn;
        fileIn.open(fileName);
        
        string tempSize;
        getline(fileIn, tempSize);
        
        if (tempSize != " ") {
            size = stoi(tempSize);
            cout << "Reading " << size << " data" << endl;
        } else {
            cout << "Reading " << size << " data" << endl;
        }
        
        fileIn.close();
    } catch (exception e) {
        cout << "There's no data to read" << endl;
    }
    
    return size;
}

//convert int to rupiah
string toRupiah(int angka, string dot = ",") {
    string output = to_string(angka);
    int inspost = output.length() - 3;
    while (inspost > 0) {
        output.insert(inspost, dot);
        inspost -= 3;
    }
    return output;
}

//read data on stok.dat
void readData(const string &fileName, barang obyekBarang[]) {
    int size = readSizeData(stockFileName);
    try {
        ifstream fileIn;
        fileIn.open(fileName);
        fileIn.ignore(10, '\n');
        for (int i = 0; i < size; ++i) {
            string tempHarga, tempStok;
            getline(fileIn, obyekBarang[i].nama);
            getline(fileIn, tempHarga);
            getline(fileIn, tempStok);
            
            obyekBarang[i].harga = stoi(tempHarga);
            obyekBarang[i].stok = stoi(tempStok);
        }
        fileIn.close();
    } catch (exception e) {
        cout << "Error while reading data" << endl;
    }
    
}

//save data on stok.dat
void saveData(const string &fileName, barang obyekBarang[], int optionSave) {
    try {
        int size = readSizeData(stockFileName);
        if (optionSave == 1) {
            size++;
        }
        cout << "Loading";
        ofstream fileOut(fileName);
        fileOut << size << endl;
        for (int i = 0; i < size; ++i) {
            fileOut << obyekBarang[i].nama << endl;
            fileOut << obyekBarang[i].harga << endl;
            fileOut << obyekBarang[i].stok << endl;
            cout << "........";
        }
        fileOut.close();
        cout << "Data saved" << endl;
    } catch (exception_ptr e) {
        cout << "Error while saving data" << endl;
    }
}

//input data to stok.dat
void inputData(barang obyekBarang[]) {
    string nama;
    cout << "Masukkan nama barang : ";
    getline(cin >> ws, nama);
    
    int harga;
    cout << "Masukkan harga barang : ";
    cin >> harga;
    
    if (!cin.good()) {// validasi variabel harga
        cin.clear(harga);
    } else {
        int stok;
        cout << "Masukkan stok barang : ";
        cin >> stok;
        
        if (!cin.good()) {// validasi variabel stok
            cin.clear(stok);
        } else {
            try {
                int size = readSizeData(stockFileName);
                obyekBarang[size].nama = nama;
                obyekBarang[size].harga = harga;
                obyekBarang[size].stok = stok;
                
                thread newThread(saveData, stockFileName, obyekBarang, 1);
                //create new thread for saving data to avoid memory leak & error pointer
                newThread.join();
                //waiting for previous thread finish, to join main thread again
            } catch (exception
                     e) {
                cout << "something error with saving data process";
            }
        }
    }
}

//show data from stok.dat
void showData(barang obyekBarang[]) {
    int size = readSizeData(stockFileName);
    
    cout << lineSeparator << endl;
    cout << "                  Stok Barang                 " << endl;
    cout << "----------------------------------------------" << endl;
    cout << "| No     | Nama         | Harga      | Stok   |" << endl;
    cout << "----------------------------------------------" << endl;
    for (int i = 0; i < size; ++i) {
        cout << setw(5) << i + 1 << setw(12);
        cout << obyekBarang[i].nama << right << setw(12);
        cout << "Rp." << toRupiah(obyekBarang[i].harga) << right << setw(10);
        cout << obyekBarang[i].stok << right << setw(10) << endl;
        cout << " " << endl;
    }
    cout << lineSeparator << endl;
    
}

//save history transaction
void saveHistoryTransaction(const string &filename, int harga) {
    try {
        ofstream fileOut(filename + ".txt", ios_base::app);
        fileOut << getCurentDateTime() << userName << right << setw(10)
        << "Rp." << toRupiah(harga) << right << setw(10) << endl << endl;
        fileOut.close();
        cout << "Data transaction saved" << endl;
    } catch (exception e) {
        cout << "Error while saving data" << endl;
    }
}

//Transaction Operation
int sumTransaction(barang itemTransaksi[], int size) {
    int totalHargaTemp = 0;
    
    for (int i = 0; i < size; ++i) {
        totalHargaTemp = totalHargaTemp + (itemTransaksi[i].harga * itemTransaksi[i].stok);
    }
    
    return totalHargaTemp;
}

//sorting transaction
void sortDataTransaction(barang itemTransaksi[], int size) {
    barang sortTemp;
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < i; ++j) {
            if (itemTransaksi[j].nama > itemTransaksi[j + 1].nama) {
                sortTemp = itemTransaksi[j];
                itemTransaksi[j] = itemTransaksi[j + 1];
                itemTransaksi[j + 1] = sortTemp;
            }
        }
    }
}

//print transaction
void printTransaction(barang itemTransaksi[], int size, pembayaran paymentTransaction) {
    system("cls");
    sortDataTransaction(itemTransaksi, size);
    cout << lineSeparator << endl;
    cout << "                               Total Belanja                                 " << endl;
    cout << "-----------------------------------------------------------------------------" << endl;
    cout << "| No     | Nama         | Jumlah      | Harga Satuan        | Total Harga   |" << endl;
    cout << "-----------------------------------------------------------------------------" << endl;
    for (int i = 0; i < size; ++i) {
        cout << setw(5) << i + 1 << setw(12);
        cout << itemTransaksi[i].nama << right << setw(13);
        cout << itemTransaksi[i].stok << right << setw(13);
        cout << "Rp." << toRupiah(itemTransaksi[i].harga) << right << setw(18);
        cout << "Rp." << toRupiah(itemTransaksi[i].harga * itemTransaksi[i].stok) << endl;
        cout << " " << endl;
    }
    cout << "----------------------------------------------------" << endl;
    cout << "Total Harga    : Rp." << toRupiah(paymentTransaction.totalHarga) << endl;
    cout << "Dibayar        : Rp." << toRupiah(paymentTransaction.totalBayar) << endl;
    cout << "Kembalian      : Rp." << toRupiah(paymentTransaction.kembalian) << endl << endl;
    cout << getCurentDateTime() << "User: " << userName << endl;
    cout << lineSeparator << endl;
    getchar();
    getchar();
}

//payment process
pembayaran transactionPayment(int totalHarga) {
    pembayaran paymentTransactionTemp{};
    paymentTransactionTemp.totalHarga = totalHarga;
    
    cout << lineSeparator << endl;
    int totalMoney;
    cout << "Bayar : Rp.";
    cin >> totalMoney;
    paymentTransactionTemp.totalBayar = totalMoney;
    
    cout << "Kembali : Rp." << toRupiah(totalMoney - totalHarga) << endl;
    paymentTransactionTemp.kembalian = totalMoney - totalHarga;
    
    return paymentTransactionTemp;
}

//checking data transaction
void transactionCheckData(barang listBarang[], barang selectedBarang[], int size) {
printout_check_transaction_data:
    system("clear");
    cout << lineSeparator << endl;
    cout << "                   Total Belanja                    " << endl;
    cout << "----------------------------------------------------" << endl;
    cout << "| No     | Nama         | Jumlah      | Total Harga " << endl;
    cout << "----------------------------------------------------" << endl;
    for (int i = 0; i < size; ++i) {
        cout << setw(5) << i + 1 << setw(12);
        cout << selectedBarang[i].nama << right << setw(14);
        cout << selectedBarang[i].stok << right << setw(10);
        cout << "Rp." << toRupiah(selectedBarang[i].harga * selectedBarang[i].stok) << right << setw(10) << endl;
        cout << " " << endl;
        
    }
    cout << "----------------------------------------------------" << endl;
    int totalHarga = sumTransaction(selectedBarang, size);
    cout << "Total Harga Yang Harus Dibayar : Rp." << toRupiah(totalHarga) << endl;
    cout << getCurentDateTime() << "User: " << userName << endl;
    cout << lineSeparator << endl;
    
    cout << "Apakah data sudah benar ? " << endl;
    cout << " 1.Ya" << endl;
    cout << " 2.Tidak" << endl;
    
    char pilihanTemp;
    cin >> pilihanTemp;
    pembayaran paymentTransaction{};
    
    switch (pilihanTemp) {
        case '1' :
            paymentTransaction = transactionPayment(totalHarga);
            printTransaction(selectedBarang, size, paymentTransaction);
            saveData(stockFileName, listBarang, 0);
            saveHistoryTransaction(getCurentDate(), totalHarga);
            break;
        case '2' :
            for (int i = 0; i < size; ++i) {
                listBarang[i].stok += selectedBarang[i].stok;
            }
            saveData(stockFileName, listBarang, 0);
            break;
        default:
            cout << "Something wrong with the input" << endl;
            goto printout_check_transaction_data;
            
    }
}

//Transaction Menu
void transactionMenu(barang listBarang[]) {
    showData(listBarang);
    int totalDataTemp = 1;
    barang selectedBarang[totalDataTemp];
    
    for (int i = 0; i < totalDataTemp; ++i) {
    add_item:
        cout << "Masukkan id Barang : ";
        int idTemp;
        cin >> idTemp;
        if (!cin.good()) { //Validasi inputan
            cout << "inputan salah" << endl;
            cin.clear(idTemp);
        } else { //validasi stok
            if (listBarang[idTemp - 1].stok == 0) {
                cout << "Stok kosong" << endl;
                goto add_item;
            } else {
                selectedBarang[i] = listBarang[idTemp - 1];
            }
        }
        
        cout << "Masukkan jumlah Barang : ";
        int stockTemp;
        cin >> stockTemp;
        if (!cin.good()) { //Validasi inputan
            cout << "inputan salah" << endl;
            cin.clear(stockTemp);
        } else { //validasi stok
            if (listBarang[idTemp - 1].stok - stockTemp < 0) {
                cout << "Stok kosong" << endl;
                goto add_item;
            } else {
                selectedBarang[i] = listBarang[idTemp - 1];
            }
        }
        selectedBarang[i].stok = stockTemp;
        listBarang[idTemp - 1].stok = listBarang[idTemp - 1].stok - stockTemp;
        
        cout << selectedBarang[i].nama << " Dimasukkan kedalam keranjang" << endl;
        cout << lineSeparator << endl << endl;
        
    add_more_item:
        cout << "Apakah ada item lainnya ?" << endl;
        cout << " 1.Ya" << endl;
        cout << " 2.Tidak" << endl;
        cout << lineSeparator << endl;
        char pilihanTemp;
        cin >> pilihanTemp;
        
        switch (pilihanTemp) {
            case '1' :
                totalDataTemp++;
                break;
            case '2' :
                transactionCheckData(listBarang, selectedBarang, totalDataTemp);
                break;
            default:
                cout << "Something wrong with the input" << endl;
                goto add_more_item;
        }
    }
}

//showing history transaction based on date
void showHistoryTransaction() {
    string filename;
    cout << lineSeparator << endl;
    cout << "Masukkan tanggal transaksi" << endl;
    cout << "contoh : 24-12-2018" << endl;
    cout << lineSeparator << endl;
    cout << "Tanggal transaksi : ";
    cin >> filename;
    try {
        ifstream fileIn;
        fileIn.open(filename + ".txt");
        while (!fileIn.eof()) {
            string dataTransaction;
            getline(fileIn, dataTransaction);
            cout << dataTransaction << endl;
        }
        fileIn.close();
        cout << lineSeparator << endl;
    } catch (bad_exception e) {
        cout << "Data tidak ditemukan pastikan tanggal yang anda input benar" << endl;
    }
}

//Main Menu
void showMenu(barang dbBarang[]) {
    cout << lineSeparator << endl;
    cout << getCurentDateTime() << "User: " << userName << endl << endl;
    cout << "     PROGRAM KASIR SEDERHANA       "
    "\n     1.Stok Barang               "
    "\n     2.Entri Barang              "
    "\n     3.Transaksi                 "
    "\n     4.Log Transaksi             " << endl;
    cout << lineSeparator << endl;
    cout << "Masukkan Pilihan : ";
    
    char pilihan;
    cin >> pilihan;
    
    switch (pilihan) {
        case '1' :
            showData(dbBarang);
            break;
        case '2' :
            inputData(dbBarang);
            break;
        case '3' :
            transactionMenu(dbBarang);
            break;
        case '4' :
            showHistoryTransaction();
            break;
        default:
            cout << "Something error with the input" << endl;
            showMenu(dbBarang);
            break;
    }
}

void showLoginMenu() {
    string userData[2][2] = {"1", "Ani", "2", "Budi"};
    cout << lineSeparator << endl;
    cout << "| Id    | Nama          |" << endl;
    for (auto &j : userData) {
        cout << setw(4) << j[0] << right << setw(10);
        cout << j[1] << endl;
    }
    cout << lineSeparator << endl;
    
user_login:
    cout << "Masukkan id User Anda : ";
    cin >> userName;
    if (userName != "1" && userName != "2") {
        cout << "User tidak ditemukan" << endl;
        goto user_login;
    }
    for (auto &i : userData) {
        if (i[0] == userName) {
            userName = i[1];
        }
    }
}

//Initialization
int main() {

     showLoginMenu();
       while (true) {
           int jumlahDataBarang = readSizeData(stockFileName);
           barang mBarang[jumlahDataBarang];
           readData(stockFileName, mBarang);
           showMenu(mBarang);
       }
}
