#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "include/caesar_dec.h"
#include "include/caesar_enc.h"
#include "include/subst_dec.h"
#include "include/subst_enc.h"
#include "utils.h"

using namespace std;

// Initialize random number generator in .cpp file for ODR reasons
std::mt19937 Random::rng;

const string ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

// Function declarations go at the top of the file so we can call them
// anywhere in our program, such as in main or in other functions.
// Most other function declarations are in the included header
// files.

// When you add a new helper function, make sure to declare it up here!
void decryptSubstCipherFileCommand(const QuadgramScorer& scorer);

/**
 * Print instructions for using the program.
 */
void printMenu();

int main() {
  Random::seed(time(NULL));
  string command;

  string word;
  vector<string> dict;
  ifstream file("dictionary.txt");
  while(getline(file, word)){
    dict.push_back(word);
  }
  file.close();

  vector<string> quadgrams;
  vector<int> counts;
  ifstream quadFile("english_quadgrams.txt");

  string line;
  while (getline(quadFile, line)) {
    stringstream ss(line);
    string quad;
    string countStr;
    if (getline(ss, quad, ',') && getline(ss, countStr)) {
      quadgrams.push_back(quad);
      counts.push_back(stoi(countStr));
    }
  }
  quadFile.close();
  QuadgramScorer scorer(quadgrams, counts);



  
  cout << "Welcome to Ciphers!" << endl;
  cout << "-------------------" << endl;
  cout << endl;

  do {
    printMenu();
    cout << endl << "Enter a command (case does not matter): ";

    // Use getline for all user input to avoid needing to handle
    // input buffer issues relating to using both >> and getline
    getline(cin, command);
    cout << endl;
    
    if(command == "C" || command == "c"){
      caesarEncryptCommand();
    }
    if(command == "D" || command == "d"){
      caesarDecryptCommand(dict);
    }
    if(command == "A" || command == "a"){
      applyRandSubstCipherCommand();
    }
    if (command == "E" || command == "e") {
      computeEnglishnessCommand(scorer);
    }
    if(command == "S" || command == "s"){
      decryptSubstCipherCommand(scorer);
    }
    if (command == "F" || command == "f") {
      decryptSubstCipherFileCommand(scorer);
    }



    if (command == "R" || command == "r") {
      string seed_str;
      cout << "Enter a non-negative integer to seed the random number "
              "generator: ";
      getline(cin, seed_str);
      Random::seed(stoi(seed_str));
    }

    cout << endl;

  } while (!(command == "x" || command == "X") && !cin.eof());

  return 0;
}

void printMenu() {
  cout << "Ciphers Menu" << endl;
  cout << "------------" << endl;
  cout << "C - Encrypt with Caesar Cipher" << endl;
  cout << "D - Decrypt Caesar Cipher" << endl;
  cout << "E - Compute English-ness Score" << endl;
  cout << "A - Apply Random Substitution Cipher" << endl;
  cout << "S - Decrypt Substitution Cipher from Console" << endl;
  cout << "F - Decrypt Substitution Cipher from File" << endl;
  cout << "R - Set Random Seed for Testing" << endl;
  cout << "X - Exit Program" << endl;
}

// "#pragma region" and "#pragma endregion" group related functions in this file
// to tell VSCode that these are "foldable". You might have noticed the little
// down arrow next to functions or loops, and that you can click it to collapse
// those bodies. This lets us do the same thing for arbitrary chunks!
#pragma region CaesarEnc

char rot(char c, int amount) {
  int i = ALPHABET.find(c);  // Find index of char.
  return ALPHABET[(amount + i) % 26]; //apply rotation (%26 allows it to wrap around) and return the encrypted char.
}

string rot(const string& line, int amount) {
  string str = "";
  for(char c: line){
    if(isalpha(c)){
      str += rot(toupper(c), amount); //Convert to uppercase to campare to ALPHABET string, and rotate by amount;
    }
    else if(isspace(c)){ //If char is a space then just add it to str;
      str += c;
    }
  }
  
  
  return str;
}

void caesarEncryptCommand() {
  string strAmount;
  string usertext;
  string cipher;
  cout << "Enter the text to encrypt:";
  getline(cin, usertext);
  cout << "Enter the number of characters to rotate by:";
  getline(cin, strAmount);
  int amount = stoi(strAmount);

  cipher = rot(usertext, amount);

  cout << "Cipher Text: " << cipher << endl;

}

#pragma endregion CaesarEnc

#pragma region CaesarDec

void rot(vector<string>& strings, int amount) {
  for(string& str: strings){
    str = rot(str, amount);
  }
}

string clean(const string& s) {
  string cleanStr = "";
  for(char c: s){
    if(isalpha(c)){
      cleanStr += toupper(c);
    }

  }
  return cleanStr;
}

vector<string> splitBySpaces(const string& s) {
  vector<string> words{};
  stringstream ss(s);
  string word = "";
  while(ss >> word){
    words.push_back(clean(word));
  }


  return words;
}

string joinWithSpaces(const vector<string>& words) {
  string str = ""; 
  for(int i = 0; i < words.size(); i++){
    str += words[i];
    if(i<words.size()-1){
      str += " ";
    }
    
  }
  return str;
}

int numWordsIn(const vector<string>& words, const vector<string>& dict) {
  int count = 0;
  for(string word: words){
    for(string d: dict){
      if(word == d){
        count++;
        break;
      }
    }
  }

  return count;
}

void caesarDecryptCommand(const vector<string>& dict) {
  string ciphertext = "";
  cout << "Enter the text to decrypt:";
  getline(cin, ciphertext);

  vector<string> ciphervec = splitBySpaces(ciphertext);
  bool found = false;


  for(int i = 0; i < 26; i++){
    vector<string> words = ciphervec;
    rot(words, i);


    int matches = numWordsIn(words, dict);
    if(matches > (int)words.size()/2){
      cout << joinWithSpaces(words) << endl;
      found = true;
    }
  }
  
  if(!found){
    cout << "No good decryptions found" << endl;
  }
}

#pragma endregion CaesarDec

#pragma region SubstEnc

string applySubstCipher(const vector<char>& cipher, const string& s) {
  string str = "";
  if(cipher.size() < 26) {
    throw std::runtime_error("Cipher must have 26 letters");
  } 
  for(char c: s){
    if(isalpha(c)){
      char upper = toupper(c);
      int i = upper - 'A';
      str += cipher[i];
    }
    else str += c;
  }

  return str;
}

void applyRandSubstCipherCommand() {
  string str;
  cout << "Enter text to encrypt:";
  getline(cin, str);

  vector<char> cipher = genRandomSubstCipher();

  

  cout << "Ciphertext: " << applySubstCipher(cipher, str) << endl;
}

#pragma endregion SubstEnc

#pragma region SubstDec

double scoreString(const QuadgramScorer& scorer, const string& s) {
  string str;
  double score = 0.0;
  for(char c: s){
    if(isalpha(c)) str += toupper(c);
  }

  for(int i = 0; i + 3 < str.size(); i++){
    string quad = str.substr(i, 4);
    score += scorer.getScore(quad);
  }

  return score;

}

void computeEnglishnessCommand(const QuadgramScorer& scorer) {
  string str = "";
  cout << "Enter a string for scoring:";
  getline(cin, str);

  cout << scoreString(scorer, str) << endl;

}

vector<char> hillClimb(const QuadgramScorer& scorer, const string& ciphertext) {
  vector<char> bestCipher = genRandomSubstCipher();
  string bestDecrypt = applySubstCipher(bestCipher, ciphertext);
  double bestScore = scoreString(scorer, bestDecrypt);
  int f = 0;
  while(f < 1000){
    vector<char> c = bestCipher;
    int i = Random::randInt(25);
    int j = Random::randInt(25);
    while (i == j) j = Random::randInt(25);
    swap(c[i], c[j]);
    
    string newChipher = applySubstCipher(c, ciphertext);
    double score = scoreString(scorer, newChipher);

    if(score > bestScore){
      bestCipher = c;
      bestScore = score;
      f = 0;
    }
    else{
      f++;
    }
  }


  return bestCipher;
}

vector<char> decryptSubstCipher(const QuadgramScorer& scorer, const string& ciphertext) {
  vector<char> bestCipher;
  double bestScore = -1e10;

  for(int i = 0; i < 50; i++){
    vector<char> c = hillClimb(scorer, ciphertext);
    string decrypt = applySubstCipher(c, ciphertext);
    double score = scoreString(scorer, decrypt);

    if(score > bestScore){
      bestScore = score;
      bestCipher = c;
    }
  }


  return bestCipher;
}

void decryptSubstCipherCommand(const QuadgramScorer& scorer) {
  string ciphertext = "";
  cout << "Enter ciphertext to decrypt:";
  getline(cin, ciphertext);

  vector<char> bestCipher = decryptSubstCipher(scorer, ciphertext);
  string plaintext = applySubstCipher(bestCipher, ciphertext);

  cout << "Decrypted text: " << plaintext << endl;


}

void decryptSubstCipherFileCommand(const QuadgramScorer& scorer) {
    string inputFilename;
    string outputFilename;
    
    cout << "Enter input filename: ";
    getline(cin, inputFilename);
    
    cout << "Enter output filename: ";
    getline(cin, outputFilename);
    ifstream inputFile(inputFilename);
    if (!inputFile.is_open()) {
        cout << "Error: Could not open input file '" << inputFilename << "'" << endl;
        return;
    }
    
    string ciphertext;
    string line;
    while (getline(inputFile, line)) {
        ciphertext += line + "\n";
    }
    inputFile.close();

    
    
    vector<char> bestCipher = decryptSubstCipher(scorer, ciphertext);
    string plaintext = applySubstCipher(bestCipher, ciphertext);
    

    ofstream outputFile(outputFilename);
    if (!outputFile.is_open()) {
        cout << "Error: Could not open output file '" << outputFilename << "'" << endl;
        return;
    }
    
    outputFile << plaintext;
    outputFile.close();
    
    cout << "Decryption complete. Output written to '" << outputFilename << "'" << endl;
}

#pragma endregion SubstDec
