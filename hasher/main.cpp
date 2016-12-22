//
//  main.cpp
//  CaptchaBot
//
//  Created by John Jin Woong Kim on 12/18/16.
//  Copyright © 2016 John Jin Woong Kim. All rights reserved.
//

#include <cstdio>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#define W 8
#define H 10
#define C_OFFSET 65
#define I_OFFSET 48
#define L_OFFSET 55

#define START_ROW 11
#define START_COL 5
using namespace std;
bool debug = false;
struct plot{
public:
    //index 0-9 is numbers
    //     10-36 is the letters
    vector<int> p;
    plot(){
        p.resize(36);
        fill(p.begin(), p.end(), 0);
    }
    void put(char c){
        int i = (int) c;
        if( i<65) p[i-I_OFFSET] = 1;
        else p[i-L_OFFSET] = 1;
    }
    
    void pr(){
        if(debug){
            for(int i = 0; i < 10;i++)
                if(p[i] == 1)
                    cout<<(char)(i+I_OFFSET)<<" ";
            for(int i = 10; i < 36;i++)
                if(p[i] == 1)
                    cout<<(char)(i+L_OFFSET)<<" ";
            cout<<endl;
        }
        for(int i = 0; i < 10;i++)
            if(p[i] == 1)
                cout<<i<<", ";
        for(int i = 10; i < 36;i++)
            if(p[i] == 1)
                cout<<i<<", ";
        cout<<endl;
    }
    
    void pr1(){
            for(int i = 0; i < 10;i++)
                if(p[i] == 1)
                    cout<<(char)(i+I_OFFSET)<<" ";
            for(int i = 10; i < 36;i++)
                if(p[i] == 1)
                    cout<<(char)(i+L_OFFSET)<<" ";
            cout<<endl;
        }
    
};

//this is the coordinated of a BLACK pixels
struct RGB{
public:
    int r,c;
    RGB(){}
    RGB(int rr, int cc){ r = rr; c = cc; }
    void prnt(){
        cout <<"arr["<<r<<"]["<<c<<"]= ";
    }
};

struct Captcha{
public:
    string name;
    vector<vector<RGB>> pixels;//8x10xn
    bool set = false;
    bool mapped(){ return set; }
    Captcha(){
        pixels.resize(10);
    }
    void setup(vector<vector<RGB>> p, string n ){
        name = n;
        pixels.resize(10);
        cout << "setup for "<< n<<endl;
        for(int i = 0; i < p.size();i++)
            for(int j = 0; j < p[i].size();j++){
                pixels[i].push_back(p[i][j]);
            }
        set = true;
    }
    void print(){
        cout <<"-=["<< name<<"]=-"<<endl;
        for(int i = 0; i < pixels.size(); i++){cout << i<<": ";
            for(int j = 0; j < pixels[i].size(); j++){
                pixels[i][j].prnt();
            }
            cout<<endl<<endl;
        }
    }
    bool operator < (const Captcha& str) const{ return (name < str.name); }
};


string file = "/Users/JJK/Desktop/CaptchaBot/CaptchaBot/input/rgbs/input";
string file1 = "/Users/JJK/Desktop/CaptchaBot/CaptchaBot/output/output";
string kind = ".txt";

vector<int> split(string str, char delimiter) {
    vector<int> internal;
    stringstream ss(str); // Turn the string into a stream.
    string tok;
    
    while(getline(ss, tok, delimiter)) {
        internal.push_back( stoi( tok) );
    }
    
    return internal;
}

string filenamer(int i){
    string r;
    if(i<10) r+= "0";
    r+= to_string(i);
    return file+r+kind;
}
string filenamer1(int i){
    string r;
    if(i<10) r+= "0";
    r+= to_string(i);
    return file1+r+kind;
}

bool isInt(string s){
    if((int)s[0] < 58) return true;
    return false;
}

int getIndex(string s, bool b){
    if(b) return (int)s[0] - I_OFFSET;
    else return (int)s[0] - C_OFFSET;
}

int main(int argc, const char * argv[]){
    vector<Captcha> nums(10);
    vector<Captcha> letters(26);
    vector<vector<plot>> pmap;
    pmap.resize(10);
    for(int i = 0; i < 10;i++){
        for(int j = 0;j<8;j++)
            pmap[i].push_back(plot());
    }
    vector<int> pcounts(36);
    vector<bool> pflags(36);
    for (int i = 0; i < 36;i++){
        pcounts[i] = 0;
        pflags[i] = false;
    }
    
    
    vector<string> outs;
    ifstream fin;
    ofstream fout;
    string input;
    vector<int> filecounts;
    
    for(int i = 0; i < 25; i++){
        string str;
        string fname = filenamer1(i);
        fin.open(fname);
        getline (fin,str);
        //cout << str << " added to outs"<<endl;
        outs.push_back(str);
        fin.close();
    }
    
    for(int i = 0; i < 25; i++){
        int filecount = 0;
        string str;
        string fname = filenamer(i);
        cout<<"==================================================="<<endl;
        cout << "Parsing file " << fname<<endl;
        fin.open(fname);
        if( fin.fail() )
            cout << "File failed to open." << endl;
        else{
            if(debug){
                cout<<"==================================================="<<endl;
                cout<<"file " << fname<<endl;
            }
            getline (fin,str);//get the line with the row and col sizes
            if(debug) printf("%s\n",str.c_str());
            
            
            char cbuf[5];
            vector<int> tpcounts = {0,0,0,0,0};
            string curr_cap(outs[i]);
            curr_cap.copy(cbuf, 5);
            vector<vector<plot>> currmap;
            currmap.resize(10);
            for(int i = 0; i < 10;i++){
                for(int j = 0;j<8;j++)
                    currmap[i].push_back(plot());
            }
            vector<vector<vector<RGB>>> vect(5);
            for(int r =0; r < 21;r++){
                int cap_index = 0;
                int cap_index_length = 0;

                for(int c = 0; c < 60;c++){
                    fin>>str;
                    //cout <<str<<endl;
                    if( c > 4 ){
                        vector<int> omfg;
                        omfg = split(str, ',');
                        if( omfg[0] < 20&&omfg[1] < 20&&omfg[2] < 20 ){
                            //cout<<"char "<<cbuf[cap_index]<<" found at " <<r<<","<<c << " -> "<<r-11<<","<<(c-(5+cap_index))%8  <<endl;
                            pmap[r-11][(c-(5+cap_index))%8].put( cbuf[cap_index] );
                            filecount++;
                            tpcounts[cap_index]++;
                        }
                        if((c!=13&&c!=22&&c!=31&&c!=40 )) cap_index_length++;
                        if(cap_index_length > 7){ cap_index++; cap_index_length = 0;}
                    }
                }
                cap_index = 0;
                cap_index_length = 0;
            }
            /*
            for(int x = 0;x<5;x++){
                int pcountint = (int)cbuf[x];
                if(pcountint<65){
                    if( pflags[pcountint-48] == false){
                        pcounts[pcountint-48] = tpcounts[x];
                        pflags[pcountint-48] = true;
                    }
                }else{
                    if( pflags[pcountint-55] == false){
                        pcounts[pcountint-55] = tpcounts[x];
                        pflags[pcountint-55] = true;
                    }
                }
            }*/
            filecounts.push_back(filecount);
            fin.close();
            cout<<"R : C"<<endl;
            for (int i = 0; i<10; i++) {
                for (int j = 0; j<8; j++) {
                    cout <<"curr["<<i<<"]["<<j<<"]= {";
                    currmap[i][j].pr1();
                    cout<<"};"<<endl;
                    //for (int c = 0;c<pmap[i][j].p.size();c++){
                    //    if(pmap[i][j].p[c] == 1) pcounts[c]++;
                    //}
                    
                }
            }

        }//closes else case
    }//closes for loop
    cout<<"R : C"<<endl;
    for (int i = 0; i<10; i++) {
        for (int j = 0; j<8; j++) {
            cout <<"arr["<<i<<"]["<<j<<"]= {";
            pmap[i][j].pr();
            cout<<"};"<<endl;
            for (int c = 0;c<pmap[i][j].p.size();c++){
                if(pmap[i][j].p[c] == 1) pcounts[c]++;
            }
            
        }
    }
    for(int i = 0;i < 36;i++) cout <<i<<" ";
    cout<<endl;
    for(int i = 0;i < 36;i++)
        cout<< pcounts[i]<<", ";
    cout<<endl;
    for(int f: filecounts)
        cout<<f<<" ";
    cout <<endl;
    cout<<"wtf"<<endl;
    return 0;
}












