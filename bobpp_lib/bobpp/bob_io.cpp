/*
 * =====================================================================================
 *
 *       Filename:  bob_io.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  20.12.2007 15:35:27
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Bertrand Le Cun (blc), blec_AT_prism.uvsq.fr
 *        Company:  PRiSM Laboratory
 *
 * =====================================================================================
 */
#include<bobpp>

namespace Bob {

void Pack(Serialize &s, const bool *t, int nb) {
  s.Pack(t, nb);
}
void Pack(Serialize &s, const int *t, int nb) {
  s.Pack(t, nb);
}
void Pack(Serialize &s, const unsigned int *t, int nb) {
  s.Pack(t, nb);
}
void Pack(Serialize &s, const short *t, int nb) {
  s.Pack(t, nb);
}
void Pack(Serialize &s, const unsigned short *t, int nb) {
  s.Pack(t, nb);
}
void Pack(Serialize &s, const long *t, int nb) {
  s.Pack(t, nb);
}
void Pack(Serialize &s, const unsigned long long *t, int nb) {
  s.Pack(t, nb);
}
void Pack(Serialize &s, const double *t, int nb) {
  s.Pack(t, nb);
}
void Pack(Serialize &s, const float *t, int nb) {
  s.Pack(t, nb);
}
void Pack(Serialize &s, const char *t, int nb) {
  s.Pack(t, nb);
}
void Pack(Serialize &s, const std::string *t, int nb) {
  s.Pack(t, nb);
}



void UnPack(DeSerialize &s, bool *t, int nb) {
  s.UnPack(t, nb);
}
void UnPack(DeSerialize &s, int *t, int nb) {
  s.UnPack(t, nb);
}
void UnPack(DeSerialize &s, unsigned int *t, int nb) {
  s.UnPack(t, nb);
}
void UnPack(DeSerialize &s, short *t, int nb) {
  s.UnPack(t, nb);
}
void UnPack(DeSerialize &s, unsigned short *t, int nb) {
  s.UnPack(t, nb);
}
void UnPack(DeSerialize &s, long *t, int nb) {
  s.UnPack(t, nb);
}
void UnPack(DeSerialize &s, unsigned long long *t, int nb) {
  s.UnPack(t, nb);
}
void UnPack(DeSerialize &s, double *t, int nb) {
  s.UnPack(t, nb);
}
void UnPack(DeSerialize &s, float *t, int nb) {
  s.UnPack(t, nb);
}
void UnPack(DeSerialize &s, char *t, int nb) {
  s.UnPack(t, nb);
}
void UnPack(DeSerialize &s, std::string *t, int nb) {
  s.UnPack(t, nb);
}

};
