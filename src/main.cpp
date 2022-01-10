
#include <iostream>
#include <string>

#include "core.hpp"
#include "view.hpp"


using namespace std;

/*
subliminal message
u_char packet[100];
//mac addresses
packet[12]='0x12';
packet[13]='0x34';
packet[14]='c';
//messaggio
*/

using namespace goose_game::view;

int main() {
  cout << "hello!" << endl;

  AppView av;
  av.show();


//  string in;
//  cin >> in;
  cout << "bye!" << endl;
}
