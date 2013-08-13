#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "nexus/NeXusFile.hpp"
#include <vector>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
using namespace std;
const uint32_t MAX_TOF = 10;//4999;
const uint32_t MAX_DET = 14;//6400;
const uint32_t BIN_DET = 7;//80;

typedef struct __MODULEEVT{
  uint8_t psd;
  uint32_t tof;
}Module_Evt;

typedef struct __HEADER{
  uint32_t subsecond;
  uint8_t  module;
  uint8_t  reserve2;
  uint8_t  type;
  uint8_t  header;
}Pulse_Header;

typedef struct __TIME{
  time_t second;
}Pulse_Time;

typedef struct __EVENT{
  uint8_t evt[7];
  uint8_t psd;
}Event;

typedef struct __EOP{
  uint8_t reserve2;
  uint8_t reserve3;
  uint8_t reserve4;
  uint8_t reserve5;
  uint8_t reserve6;
  uint8_t reserve7;
  uint8_t reserve8;
  uint8_t eop;
}EndOfPulse;

uint32_t MapIdx(uint32_t tofidx, uint32_t detidx){
  return tofidx*MAX_DET+detidx;
}

uint32_t TofIdx(uint32_t mapidx){
  return mapidx/MAX_DET;
}

uint32_t DetIdx(uint32_t mapidx){
  return mapidx%MAX_DET;
}

uint8_t PSDIdx(uint32_t detidx){
  return detidx/BIN_DET;
}

uint8_t PosIdx(uint32_t detidx){
  return detidx%BIN_DET;
}

void Encode_PulseHeader(Pulse_Header* pulseHeader, uint8_t *type, uint8_t *module, uint32_t *subsecond){
  pulseHeader->header    = 0x0;
  pulseHeader->type      = *type;
  pulseHeader->module    = *module;
  pulseHeader->subsecond = *subsecond; 
}

void Decode_PulseHeader(uint64_t *buff, uint32_t *type, uint32_t *module, uint32_t *subsecond ){
  *type      = (uint32_t) (((*buff)>>48)&0xFF);
  *module    = (uint32_t) (((*buff)>>32)&0xFF); 
  *subsecond = (uint32_t) ((*buff)&0xFFFFFFFF);
}

void Encode_PulseTime(Pulse_Time* pulseTime, time_t *second){
  pulseTime->second    = *second; 
}

void Decode_PulseTime(uint64_t *buff, time_t *second ){
  *second = (time_t) (* buff); 
}

void Encode_Event(Event* event, uint8_t *psd, uint32_t *tof, uint32_t *qa, uint32_t *qb){
  event->psd = *psd;
  event->evt[6] = ( (*tof) >>20); 
  event->evt[5] = ( (*tof) >>12); 
  event->evt[4] = ( (*tof) >>4); 
  event->evt[3] = (((*tof) & 0xF  )<<4) + (((*qa) & 0x3C00)>>10); 
  event->evt[2] = (((*qa)  & 0x3FC)>>2); 
  event->evt[1] = (((*qa)  & 0x3  )<<6) + (((*qb) & 0x3F00)>>8); 
  event->evt[0] = ((*qb) & 0xFF); 
}

void Decode_Event(uint64_t *buff, uint32_t *psd, uint32_t *tof, uint32_t *qa, uint32_t *qb ){
  *psd = (uint32_t) (((*buff) >> 56 ) & 0xFF);
  *tof = (uint32_t) (((*buff) >> 28 ) & 0xFFFFFFF);
  *qa  = (uint32_t) (((*buff) >> 14 ) & 0x3FFF);
  *qb  = (uint32_t) (( *buff) & 0x3FFF);
}

void Encode_EOP(EndOfPulse* eop){
  eop->eop = 0xFF;
}

void SaveNexusFile(){
  /*----------------------------------------------*/
  NeXus::File file("test.nxs",NXACC_CREATE5);
  file.makeGroup("entry","NXentry",true);
  file.closeGroup();
}

void SaveHeaderToBinaryFile(ofstream& fout, uint8_t *type, uint8_t *module, uint32_t *subsecond) {
  /*----------------------------------------------*/
  Pulse_Header* pulseHeader = new Pulse_Header;
  Encode_PulseHeader(pulseHeader, type, module, subsecond);
  fout.write((char*)pulseHeader, sizeof(Pulse_Header));
  delete pulseHeader;
}

void SaveTimeStampToBinaryFile(ofstream& fout, time_t *second){
  /*----------------------------------------------*/
  Pulse_Time* pulseTime = new Pulse_Time;
  Encode_PulseTime(pulseTime, second);
  fout.write((char*)pulseTime, sizeof(Pulse_Time));
  delete pulseTime;
}

void SaveEventToBinaryFile(ofstream& fout, uint8_t *PSD, uint32_t *TOF, uint32_t *QA, uint32_t *QB){
  Event* event = new Event;
  Encode_Event(event, PSD, TOF, QA, QB);
  fout.write((char*)event, sizeof(Event));
  delete event;
}

void SaveEOPToBinaryFile(ofstream& fout){
  /*----------------------------------------------*/
  EndOfPulse* eop = new EndOfPulse;
  Encode_EOP(eop); 
  fout.write((char*)eop, sizeof(EndOfPulse));
  delete eop;
}

uint32_t Get_PositionID(uint32_t qa, uint32_t qb){
  if ((qa+qb)<1){
    //std::cout << "qa " << qa << " qb " << qb << std::endl;
    return 0;
  }
  double R = (double)qa/(qa+qb);
  double P = R*BIN_DET;
  int IntP = (uint32_t)P;
  double D = P - IntP;
  if(D>0.5){
    return (IntP + 1);
  }
  else{
    return IntP;
  }
}

void SaveBinaryFile(uint32_t *cmap){
  std::cout << "SaveBinaryFile" << std::endl;
  std::ofstream fout("hh", std::ios::binary); 

  std::time_t UnixTime = std::time(0);  // t is an integer type

  time_t   second    = 100000;
  uint32_t subsecond = 123456789;
  uint8_t type   = 0x0;
  uint8_t module = 0x1;

  std::cout << "SaveHeader" << std::endl;
  SaveHeaderToBinaryFile(fout, &type, &module, &subsecond);
  std::cout << "SaveTime" << std::endl;
  SaveTimeStampToBinaryFile(fout, &second);
  /*----------------------------------------------*/
  srand((int)time(0));
  std::cout << "SaveEvent" << std::endl;
  for(int i=0; i < MAX_TOF ; i++){
    uint32_t TOF = i;
    for(int j=0; j < MAX_DET ; j++){
      uint8_t PSD = PSDIdx(j);
      uint8_t pos = PosIdx(j);
      double R = ((double)pos)/BIN_DET;
      //std::cout << "(" <<(int)PSD << "/" << (int)pos << "/" << (int)TOF << ") ";
      for(int k=0;k<cmap[MapIdx(i,j)];k++){
	uint32_t QB = 0;
	uint32_t QA = 0;
	if(R<0.5){
	  QB = (rand()%1024+1);
	  QA = R*QB/(1-R);
	}
	else{
	  QA = (rand()%1024+1);
	  QB = QA*(1-R)/R;
	}
	//std::cout << "pos " << (int)pos << " getpos " << Get_PositionID(QA,QB)<< std::endl;
	if(pos!=Get_PositionID(QA,QB)){
	  std::cout << "pos " << (int)pos  << " double " << (double)pos << " R " << R 
	    << " QA " << QA << " QB " << QB <<" GetPos " << Get_PositionID(QA,QB) << std::endl;
	}
	SaveEventToBinaryFile(fout, &PSD, &TOF, &QA, &QB);
      }
    }
   // std::cout << std::endl;
  }
  SaveEOPToBinaryFile(fout);


  /*----------------------------------------------*/
  fout.close();
}

void Map_EventToDetector(uint32_t *dmap, uint32_t *module, uint32_t *psd, uint32_t *tof, uint32_t *qa, uint32_t *qb){
  uint32_t pos_id = Get_PositionID(*qa, *qb);
  uint32_t det_id = (*psd) * BIN_DET + pos_id;
  uint32_t id = (*tof) * MAX_DET + det_id;
  //std::cout << "(" << *psd << "/" << pos_id << "/" << det_id << "/" << *tof << "/"<<id << ")" << std::endl;
  dmap[id] += 1;
  //std::cout << " Map Event: module " << *module << " psd " << *psd 
  //  << " tof " << *tof << " qa "  << *qa << " qb " << *qb  
  //  << " pos id " << pos_id  << " det id " << det_id 
  //  << " Matrix " << id << std::endl; 
}

void PrintDMap(uint32_t *dmap){
  std::ofstream outfile("txt");
  for (uint32_t i = 0; i < MAX_TOF; i++ ){
    for (uint32_t j = 0; j < MAX_DET; j++ ){
      outfile << dmap[i*MAX_DET+j] << ";";
    }
    outfile << std::endl;
  }
}

void Decode_RawDataSegment(uint64_t *Buff, uint32_t *dmap, uint32_t size, uint8_t *flag){
  std::cout << "Enter Decode_RawDataSegment(), buffer size: " << size << std::endl;
  uint64_t *ReadRawData;// = new uint8_t[8]; 
  time_t second;
  uint32_t type, module, subsecond;
  for (uint32_t i = 0; i < size ; i++ ){
    ReadRawData = (uint64_t*)(Buff+i);
    //std::cout << "idx " << i << " " << std::hex << *ReadRawData << std::endl;// << std::endl;
    //std::cout << "zzz: " << std::hex << *ReadRawData << std::endl;
    if ((((*ReadRawData)>>56) == 0x0) && (*flag == 0))  {
      Decode_PulseHeader(ReadRawData, &type, &module, &subsecond);
      std::cout << " Header: type " << type << " module  " << module << " subsecond " << subsecond << std::endl;
      *flag = 1;
    }
    else if (*flag == 1){
      //int64_t second;
      Decode_PulseTime(ReadRawData, &second);
      std::cout << " Time: second " << second  <<" "<< ctime((time_t*)&second)    <<  std::endl; 
      *flag = 2; 
      continue;
    }
    if ((((*ReadRawData)>>56) == 0xFF)&&(*flag >= 2)) {
      std::cout << " EndOfPulse" << std::endl; 
      *flag = 0;
    }
    else if ((*flag == 2)||(*flag == 3)){
      uint32_t psd, tof, qa, qb;
      Decode_Event(ReadRawData, &psd, &tof, &qa, &qb);
      if ((qa+qb)<1){
	std::cout << "decode qa " << qa << " qb " << qb << std::endl;
      }
      Map_EventToDetector(dmap, &module,&psd, &tof, &qa, &qb );
      *flag = 3;
    }
  }
}


void LoadBinaryFile(uint32_t *dmap){

  /*----------------------------------------------*/
  uint32_t size = 1000;
  uint8_t *flag = new uint8_t;
  *flag = 0;
  size_t buffsize = 0; 
  uint64_t *Buff = new uint64_t[size]; 
  std::ifstream fin("hh", std::ios::binary);
  fin.read((char*)Buff, sizeof(uint64_t)*size);
  buffsize = fin.gcount();  
  Decode_RawDataSegment(Buff, dmap, size, flag);
  while (buffsize == (sizeof(uint64_t)*size)){
    std::cout << "LoadBinaryFile " << fin.gcount() << std::endl;
    fin.read((char*)Buff, sizeof(uint64_t)*size);
    buffsize = fin.gcount();  
    if ((sizeof(uint64_t)*size) == buffsize ){
      Decode_RawDataSegment(Buff, dmap, size, flag);
    }
    else{
      std::cout << "Read file " << buffsize/(sizeof(uint64_t)) << std::endl;
      Decode_RawDataSegment(Buff, dmap, buffsize/(sizeof(uint64_t)), flag);
    }
  }
  delete flag;
  //std::cout << " raw data: "<< (uint32_t)ReadRawData[2] << " " << sizeof(time_t) << " " << sizeof(uint32_t)<< std::endl;


  fin.close();
}


void LoadSimulationFile(uint32_t* cmap){

  std::cout << "LoadSimulationFile "<< std::endl;
  //std::ifstream samplefile("/home/tianhl/workarea/CSNS_SANS_SIM/sample/sans_run/sample_sans_D.txt");
  std::ifstream samplefile("/home/tianhl/workarea/CSNS_SANS_SIM/app/test/test_raw");
  string samplebuff;
  getline(samplefile, samplebuff);
  uint32_t tot=0;

  for (int tofidx=0;tofidx<MAX_TOF ;tofidx++){
    getline(samplefile, samplebuff);
    vector<string> substring;
    vector<double> counts;
    //boost::split( substring, samplebuff, boost::is_any_of( "\t " ), boost::token_compress_on );
    boost::split( substring, samplebuff, boost::is_any_of( ";" ), boost::token_compress_on );
    //std::cout <<"Process Line " <<  tofidx << std::endl;
    for(uint32_t detidx = 0; detidx < MAX_DET  ; detidx++){
      //cmap[MapIdx(tofidx, detidx)] =(int)(atof(substring[detidx+1].c_str()))/10;
      cmap[MapIdx(tofidx, detidx)] =(int)(atoi(substring[detidx+1].c_str()));
//      std::cout << "tofidx " << tofidx << " detidx " << detidx 
//	<< " MapIdx " << MapIdx(tofidx,detidx) << std::endl;
//      std::cout << "rtof   " << TofIdx(MapIdx(tofidx,detidx))  
//	<< " rdet   " << DetIdx(MapIdx(tofidx,detidx)) << std::endl;
      tot+=(int)(atoi(substring[detidx+1].c_str()));
    }
  }
  std::cout << "total neutron hit count: " << tot << std::endl;
}

int main(int argc, char *argv[])
{
  //if ( argc != 2 ) {
  //    std::cout << "Usage: " << argv[0] << "  option.txt" << std::endl;
  //    return 1;
  //}
  uint32_t *cmap = new uint32_t[MAX_TOF*MAX_DET];
  uint32_t *dmap = new uint32_t[MAX_TOF*MAX_DET];
  LoadSimulationFile(cmap); 
  //for (int i=0;i<MAX_TOF;i++){
  //  for (int j=0;j<MAX_DET;j++){
  //    std::cout << cmap[MapIdx(i,j)]<<" ";
  //  }
  //  std::cout<<std::endl;
  //}
  SaveBinaryFile(cmap);
  LoadBinaryFile(dmap);
  //for (int i=0;i<MAX_TOF;i++){
  //  for (int j=0;j<MAX_DET;j++){
  //    std::cout << dmap[MapIdx(i,j)]<<" ";
  //  }
  //  std::cout<<std::endl;
  //}
  PrintDMap(dmap);




  //delete [] ReadRawData;
  /*----------------------------------------------*/
  //SniperMgr mgr(argv[1]);

  //if ( mgr.initialize() ) {
  //    mgr.run();
  //}

  //mgr.finalize();

  return 0;
}
