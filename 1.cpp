#include <iostream>
#include <cstdlib>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
using namespace std;

int main(){

    srand (time(NULL));
    int A = rand() % 3 + 1;//A is computer choice
    int B;
    string computerChoice;
    string userChoice;

    if (A == 1) {
    computerChoice = "rock";
} else if (A == 2) {
    computerChoice = "paper";
} else {
    computerChoice = "scissors";
}

    cout << "Pick: Rock(1), Paper(2) or Scissors(3) by typing the corresponding number" << endl;
    cin >> B;
    if (B==1){
        userChoice = "rock";
    } else if(B==2){
        userChoice = "paper";
        } else {
        userChoice = "scissors";
        }
    cout << "Your choice is: " << userChoice << endl;
    cout << "My choice is: " << computerChoice << endl;

    if (userChoice=="rock" && computerChoice=="rock"){
        cout<<"It's a tie, nobody won.\n";
    }
    if (userChoice=="paper" && computerChoice=="paper"){
        cout<<"It's a tie, nobody won.\n";
    }
    if (userChoice=="scissors" && computerChoice=="scissors"){
        cout<<"It's a tie, nobody won.\n";
    }
    if (userChoice=="rock" && computerChoice=="paper"){
        cout<<"You've lost, better luck next time.\n";//LOSE R P
     }
    if (userChoice=="paper" && computerChoice=="scissors"){
        cout<<"You've lost, better luck next time.\n";//LOSE P S
     }
    if (userChoice=="scissors" && computerChoice=="rock"){
        cout<<"You've lost, better luck next time.\n";//LOSE S R
     }
    if (userChoice=="scissors" && computerChoice=="paper"){
        cout<<"You've won, congratulations!\n";//WIN S P
     }
    if (userChoice=="paper" && computerChoice=="rock"){
        cout<<"You've won, congratulations!\n";//WIN P R
     }
    if (userChoice=="rock" && computerChoice=="scissors"){
        cout<<"You've won, congratulations!\n";//WIN R S
     }
    system("pause");
     return 0;
}
