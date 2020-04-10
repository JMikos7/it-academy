#include <iostream>
using namespace std;

string cipher(string word, int key)
{
    string cipheredWord = "";

    for (int i = 0;i <word.length() ;i++ )
    {
        if (isupper(word[i]))//for uppercase letters
        cipheredWord += char(int(word[i]+key-65)%26 +65);//65 in ASCII table is A and alphabet is 26 letters long

        else
        cipheredWord += char(int(word[i]+key-97)%26 +97);//97 is a
    }

    return cipheredWord;
}
int main(){
    string word;
    int key;
    cout << "Please enter your word: " << endl;
    cin >> word;

    cout << "Please enter by how many letters you want to shift your word: " << endl;
    cin >> key;

    cout <<"Your ciphered word is: " << cipher(word, key) << endl;


    return 0;
}
