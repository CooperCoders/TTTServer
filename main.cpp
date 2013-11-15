//Tic-Tac-Toe Server
#include <iostream>
#include <math.h>
#include <ctime>
#include <winsock2.h>
#include <sstream>
#include <string>
#include <stdlib.h>
#define MESSAGE_MAX 11

using namespace std;

char board[9];

void drawBoard();
char winCheck();
bool tieCheck();

int main()
{
  int sock;
  struct sockaddr_in serverAddr;
  struct sockaddr_in clientAddr;
  struct sockaddr_in fromAddr;
  int clientAddrLength;
  unsigned short serverPort;
  WSADATA wsaData;
  char* message;
  char messageBuffer[MESSAGE_MAX];
  stringstream ss;
  char P1, P2, input;
  int location;
  bool turn, quit = false;
  cout << "What port would you like to use?\nPort: ";
  cin >> serverPort; // selecting server port
  if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
  {
    cout << "WSAStartup() failed\n";
    exit(1);
  }
  if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
  {
    cout << "Socket creation failed\n";
    exit(1);
  }

  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serverAddr.sin_port = htons(serverPort);

  if (bind(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
  {
    cout << "Bind failed\n";
    exit(1);
  }

  cout << "Waiting for Client\n";

  clientAddrLength = sizeof(clientAddr);

  if ((recvfrom(sock, messageBuffer, MESSAGE_MAX, 0,(struct sockaddr *) &clientAddr, &clientAddrLength)) < 0)
  {
    cout << "recvfrom failed\n" << WSAGetLastError();
    exit(1);
  }

  if (messageBuffer[0] == 'C')
  {
    message = "c";
    sendto(sock, message, MESSAGE_MAX, 0, (struct sockaddr*) &clientAddr, sizeof(clientAddr)) != MESSAGE_MAX;
  }
  else
  {
    cout << "Error in connecting to client\n";
    exit(1);
  }
  cout << "Connected to " << inet_ntoa(clientAddr.sin_addr) << endl;

  while (!quit)
  {
    for (int i = 0; i < 9; i++)
      board[i] = ' ';

    cout << "Player 1 choose your character: ";
    do
    {
      cin >> P1;
    }
    while (P1 == ' ');

    ss.str(string());
    ss << "1" << P1;
    sendto(sock, ss.str().c_str(), MESSAGE_MAX, 0, (struct sockaddr*)&clientAddr, sizeof(clientAddr));

    if ((recvfrom(sock, messageBuffer, MESSAGE_MAX, 0,(struct sockaddr *) &clientAddr, &clientAddrLength)) < 0)
    {
      cout << "recvfrom failed\n" << WSAGetLastError();
      exit(1);
    }

    if (messageBuffer[0] == '2')
      cout << "Player 2 chose: " << messageBuffer[1] << endl;
    else
    {
      cout << "Message Error\n";
      exit(1);
    }

    P2 = messageBuffer[1];

    srand(time(NULL));
    turn = rand() % 2;

    ss.str(string());
    ss << "T" << turn;
    sendto(sock, ss.str().c_str(), MESSAGE_MAX, 0, (struct sockaddr*)&clientAddr, sizeof(clientAddr));

    cout << "Player " << (turn ? "1" : "2") << " will go first";

    while(winCheck() == ' ' && !tieCheck())
    {
      drawBoard();
      if (!turn)
      {
        cout << "It is Player 2's move\n";
        if ((recvfrom(sock, messageBuffer, MESSAGE_MAX, 0,(struct sockaddr *) &clientAddr, &clientAddrLength)) < 0)
        {
          cout << "recvfrom failed\n" << WSAGetLastError();
          exit(1);
        }
        if (messageBuffer[0] = 'B')
          memcpy(board, messageBuffer+1, 9);
        else
        {
          cout << "Error recieving message\n";
          exit(1);
        }
        turn= !turn;
      }
      else
      {
        cout << "Player 1, where do you want to go? (1-9)\n";
        do
        {
          cin >> location;
        }
        while (location < 1 || location > 10 || board[location - 1] != ' ');
        board[location - 1] = (turn ? P1 : P2);
        turn = !turn;
        ss.str(string());
        ss << "B" << board;
        sendto(sock, ss.str().c_str(), MESSAGE_MAX, 0, (struct sockaddr*) &clientAddr, sizeof(clientAddr));
      }
    }
    drawBoard();
    if (winCheck() == P1)
      cout << "Congratulation Player 1!\n";
    else if (winCheck() == P2)
      cout << "Congratulation Player 2!\n";
    else
      cout <<"Tie Game\n";
    cout << "Play again? (Y/N)\n";
    cin >> input;
    if (toupper(input) == 'N')
      quit = true;
  }
  closesocket(sock);
  WSACleanup();
  system("PAUSE");
  return 0;
}

void drawBoard()
{
  system("CLS");
  cout << endl;
  cout << "\t" << board[0] << (char)179 << board[1] << (char)179 << board[2]
       << "    1" << (char)179 << "2" << (char)179 << "3\n";
  cout << "\t" << (char)196 << (char)197 << (char)196 << (char)197 << (char)196
       << "    " << (char)196 << (char)197 << (char)196 << (char)197 << (char)196 << endl;
  cout << "\t" << board[3] << (char)179 << board[4] << (char)179 << board[5]
       << "    4" << (char)179 << "5" << (char)179 << "6\n";
  cout << "\t" << (char)196 << (char)197 << (char)196 << (char)197 << (char)196
       << "    " << (char)196 << (char)197 << (char)196 << (char)197 << (char)196 << endl;
  cout << "\t" << board[6] << (char)179 << board[7] << (char)179 << board[8]
       << "    7" << (char)179 << "8" << (char)179 << "9\n\n";
}

char winCheck()
{
  if (board[0] == board[1] && board[1] == board[2] && board[0] != ' ')//top
    return board[0];
  if (board[0] == board[4] && board[4] == board[8] && board[0] != ' ')//diagnal right down
    return board[0];
  if (board[0] == board[3] && board[3] == board[6] && board[0] != ' ')//left
    return board[0];
  if (board[3] == board[4] && board[4] == board[5] && board[3] != ' ')//middle h
    return board[3];
  if (board[6] == board[7] && board[7] == board[8] && board[6] != ' ')//bottom
    return board[6];
  if (board[6] == board[4] && board[4] == board[2] && board[6] != ' ')//diagnal right up
    return board[6];
  if (board[1] == board[4] && board[4] == board[7] && board[1] != ' ')//middle v
    return board[1];
  if (board[2] == board[5] && board[5] == board[8] && board[2] != ' ')//right
    return board[2];
  return ' ';
}

bool tieCheck()
{
	for (int i = 0; i < 9; i++)
  {
    if (board[i] == ' ')
      return false;
  }
  return true;
}
