/************************ OUTPUT.C **********************/
/*
 * Rory Barnes, Wed May  7 16:38:28 PDT 2014
 *
 * Subroutines for output and logging.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "vplanet.h"
#include "output.h"

/* Individual WriteOutput functions */

/*
 * A
 */

void WriteAge(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dAge;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsTime(units->iTime)
      ;
    fsUnitsTime(units->iTime,cUnit);
  }
}

/* iBodyType */
void WriteBodyType(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  if(body[iBody].bBinary) {
    *dTmp = body[iBody].iBodyType;
  }
  else
    *dTmp = -1;

  strcpy(cUnit,"");
}

/*
 * D
 */

void WriteDeltaTime(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  if (control->Evolve.dTime > 0 || control->Evolve.nSteps == 0)
    *dTmp = control->Io.dOutputTime/control->Evolve.nSteps;
  else
    *dTmp = 0;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime,cUnit);
  }
}

void WriteDensity(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdSphereDensity(body[iBody].dMass,body[iBody].dRadius);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= pow(fdUnitsLength(units->iLength),3)/fdUnitsMass(units->iLength);
    fsUnitsDensity(units,cUnit);
  }
}


/*
 * H
 */

void WriteHecc(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].dHecc;
  strcpy(cUnit,"");
}

void WriteHZLimitDryRunaway(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  /* This output is unusual in that it depends on the presence of other bodies
     in the system, namely (a) star(s). This should also only return a
     sensible value if body 0 uses module STELLAR, or else dLuminosty is
     not defined. As a hack, the will return -1 for invalid cases. */

  if (iBody == 0 || body[iBody].iBodyType == 1) // Star
    *dTmp = -1;
  else if (body[0].bStellar) { // Planet and body 0 uses module STELLAR
    *dTmp = pow(body[0].dLuminosity*(1-body[iBody].dAlbedoGlobal)/(16*PI*DRYRGFLUX*(1-body[iBody].dEcc*body[iBody].dEcc)),0.5);
  } else { // Planet, but body 0 does not use STELLAR
    *dTmp = -1;
  }

    if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength,cUnit);
  }
}

/*
 * K
 */

void WriteK2Man(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dK2Man;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}

void WriteImk2Man(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].dImk2Man;
  strcpy(cUnit,"");
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}

void WriteKecc(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].dKecc;
  strcpy(cUnit,"");
}

/*
 * L
 */

void WriteLongP(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {


  if (body[iBody].bSpiNBody) {
    *dTmp = body[iBody].dLongP;

    if (output->bDoNeg[iBody]) {
      *dTmp *= output->dNeg;
      strcpy(cUnit,output->cNeg);
    } else {
      *dTmp /= fdUnitsAngle(units->iAngle);
      fsUnitsAngle(units->iAngle,cUnit);
    }
  } else{
    *dTmp = atan2(body[iBody].dHecc, body[iBody].dKecc);

    while (*dTmp < 0.0) {
      *dTmp += 2*PI;
    }
    while (*dTmp > 2*PI) {
      *dTmp -= 2*PI;
    }

    if (output->bDoNeg[iBody]) {
      *dTmp *= output->dNeg;
      strcpy(cUnit,output->cNeg);
    } else {
      *dTmp /= fdUnitsAngle(units->iAngle);
      fsUnitsAngle(units->iAngle,cUnit);
    }
  }

}

void WriteBodyArgP(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double varpi, Omega;

  if (body[iBody].bDistOrb) {
    varpi = atan2(body[iBody].dHecc, body[iBody].dKecc);
    Omega = atan2(body[iBody].dPinc, body[iBody].dQinc);
    *dTmp = varpi - Omega;
  } else if (body[iBody].bGalHabit) {
    *dTmp = body[iBody].dArgP;
  }

  while (*dTmp < 0.0) {
    *dTmp += 2*PI;
  }
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle,cUnit);
  }
}

void WriteLXUVTot(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Multiple modules can contribute to this output */
  int iModule;

  *dTmp=0;

  if (body[iBody].bFlare)
    *dTmp += fdLXUVFlare(body,control->Evolve.dTimeStep,iBody);
  if (body[iBody].bStellar)
    *dTmp += body[iBody].dLXUV;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergyFlux(units,cUnit);
  }
}

/*
 * M
 */

void WriteMass(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].dMass;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass,cUnit);
  }
}

/*
 * O
 */

void WriteObliquity(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = atan2(sqrt(pow(body[iBody].dXobl,2)+pow(body[iBody].dYobl,2)),body[iBody].dZobl);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle,cUnit);
  }
}

void WriteBodyPrecA(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  if (body[iBody].bDistRot == 0 && body[iBody].bPoise == 1) {
    *dTmp = body[iBody].dPrecA;
  } else {
    *dTmp = atan2(body[iBody].dYobl,body[iBody].dXobl);
  }

  while (*dTmp < 0.0) {
    *dTmp += 2*PI;
  }
  while (*dTmp > 2*PI) {
    *dTmp -= 2*PI;
  }

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle,cUnit);
  }
}

void WriteOrbAngMom(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  char cTmp;
  double *pdOrbMom;
  if (body[iBody].bSpiNBody){
    pdOrbMom = fdOrbAngMom(body,iBody);
    *dTmp = sqrt(pdOrbMom[0]*pdOrbMom[0]+pdOrbMom[1]*pdOrbMom[1]+pdOrbMom[2]*pdOrbMom[2]);
    free(pdOrbMom);
  }
  else {
    pdOrbMom = fdOrbAngMom(body,iBody);
    *dTmp = *pdOrbMom;
    free(pdOrbMom);
  }
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime)/(fdUnitsMass(units->iMass)*fdUnitsLength(units->iLength)*fdUnitsLength(units->iLength));
    fsUnitsAngMom(units,cUnit);
  }
}

void WriteOrbEcc(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  if(body[iBody].bBinary != 1) { // Not doing binary
    if (iBody > 0) {
      if (body[iBody].bDistOrb || body[iBody].bEqtide) {
        *dTmp = sqrt(pow(body[iBody].dHecc,2)+pow(body[iBody].dKecc,2));
      } else {
        *dTmp = body[iBody].dEcc;
      }
    } else {
      *dTmp = -1;
    }
  }
  else // Doing binary
  {
    if(body[iBody].iBodyType == 0) // CBP
      *dTmp = sqrt(pow(body[iBody].dHecc,2)+pow(body[iBody].dKecc,2));
    else if(body[iBody].iBodyType == 1 && iBody == 1) // binary
      *dTmp = sqrt(pow(body[iBody].dHecc,2)+pow(body[iBody].dKecc,2));
    else
      *dTmp = -1;
  }
  sprintf(cUnit,"");
}

void WriteLostEng(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].dLostEng;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergy(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergy(units,cUnit);
  }
}

void WriteOrbEnergy(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdOrbEnergy(body,control,system,iBody);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergy(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergy(units,cUnit);
  }
}

void WriteOrbMeanMotion(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  if(body[iBody].bBinary == 0) { // Not doing binary
  if (iBody > 0)
    *dTmp = body[iBody].dMeanMotion;
  else
    *dTmp = -1;
  }
  else { // doing binary
  if(iBody > 0)
  {
    *dTmp = body[iBody].dMeanMotion;
  }
  else
    *dTmp = -1;
  }

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}

void WriteOrbPeriod(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  if(body[iBody].bBinary == 0) { // Not doing binary
    if (iBody > 0)
      *dTmp = fdSemiToPeriod(body[iBody].dSemi,(body[0].dMass+body[iBody].dMass));
    else
      *dTmp=-1;
  }
  else // Doing binary
  {
    if(body[iBody].iBodyType == 0) // CBP
      *dTmp = fdSemiToPeriod(body[iBody].dSemi,(body[0].dMass+body[1].dMass+body[iBody].dMass));
    else if(body[iBody].iBodyType == 1 && iBody == 1) // Binary
      *dTmp = fdSemiToPeriod(body[iBody].dSemi,(body[0].dMass+body[iBody].dMass));
    else
      *dTmp = -1;
  }

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime,cUnit);
  }
}

void WriteOrbSemi(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  if(body[iBody].bBinary == 0) { // Not doing binary
    if (iBody > 0)
      *dTmp = body[iBody].dSemi;
    else
      *dTmp = -1;
  }
  else { // Doing binary
    if(body[iBody].iBodyType == 0) // CBP
      *dTmp = body[iBody].dSemi;
    else if(body[iBody].iBodyType == 1 && iBody == 1) // Binary
      *dTmp = body[iBody].dSemi;
    else
      *dTmp = -1;
  }

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength,cUnit);
  }
}

/*
 * R
 */

void WriteRadius(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].dRadius;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength,cUnit);
  }
}

void WriteRotAngMom(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdRotAngMom(body[iBody].dRadGyra,body[iBody].dMass,body[iBody].dRadius,body[iBody].dRotRate);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime)/(fdUnitsMass(units->iMass)*fdUnitsLength(units->iLength)*fdUnitsLength(units->iLength));
    fsUnitsAngMom(units,cUnit);
  }
}

void WriteRotKinEnergy(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdRotKinEnergy(body[iBody].dMass,body[iBody].dRadius,body[iBody].dRadGyra,body[iBody].dRotRate);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergy(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergy(units,cUnit);
  }
}

void WriteRotRate(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].dRotRate;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}

void WriteRotPer(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdFreqToPer(body[iBody].dRotRate);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime,cUnit);
  }
}

void WriteRotVel(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdRotVel(body[iBody].dRadius,body[iBody].dRotRate);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime)/fdUnitsLength(units->iLength);
    fsUnitsVel(units,cUnit);
  }
}

void WriteSurfaceEnergyFlux(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  /* Surface Energy Flux is complicated because it either all comes
     through thermint, or it can be from eqtide and/or radheat. */
  *dTmp=0;

  int bOcean = 0;
  int bEnv = 0;

  // From initial conditions, did we want to model ocean tidal effects?
  if(body[iBody].dTidalQOcean < 0)
    bOcean = 0;
  else
    bOcean = 1;

  // Same as ocean, but for envelope
  if(body[iBody].dTidalQEnv < 0)
    bEnv = 0;
  else
    bEnv = 1;

  if (body[iBody].bThermint) {
    *dTmp += fdHfluxSurf(body,iBody);

    if((body[iBody].bOceanTides && bOcean) || (body[iBody].bEnvTides && bEnv))
    {
       *dTmp += fdSurfEnFluxOcean(body,iBody);
    }

  } else {
  if (body[iBody].bEqtide)
    *dTmp += fdSurfEnFluxEqtide(body,system,update,iBody,control->Evolve.iEqtideModel);
  if (body[iBody].bRadheat)
    *dTmp += fdSurfEnFluxRadTotal(body,system,update,iBody,iBody);
  }

  /* This is the old way
  *dTmp=0;
  for (iModule=0;iModule<control->Evolve.iNumModules[iBody];iModule++)
    // Only module reference in file, can this be changed? XXX
    *dTmp += output->fnOutput[iBody][iModule](body,system,update,iBody,control->Evolve.iEqtideModel);
    */
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergyFlux(units,cUnit);
  }
}

void WriteTime(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = control->Evolve.iDir*control->Evolve.dTime;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime,cUnit);
  }
}

void WriteTotAngMom(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdTotAngMom(body,control,system);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime)/(fdUnitsMass(units->iMass)*fdUnitsLength(units->iLength)*fdUnitsLength(units->iLength));
    fsUnitsAngMom(units,cUnit);
  }
}

void WriteLostAngMom(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].dLostAngMom;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime)/(fdUnitsMass(units->iMass)*fdUnitsLength(units->iLength)*fdUnitsLength(units->iLength));
    fsUnitsAngMom(units,cUnit);
  }
}

void WriteTotEnergy(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdTotEnergy(body,control,system);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergy(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergy(units,cUnit);
  }
}

void WritePotEnergy(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdPotEnergy(body,control,system,iBody);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergy(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergy(units,cUnit);
  }
}

void WriteKinEnergy(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdKinEnergy(body,control,system,iBody);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergy(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergy(units,cUnit);
  }
}

void WriteOrbKinEnergy(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdOrbKinEnergy(body,control,system,iBody);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergy(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergy(units,cUnit);
  }
}

void WriteOrbPotEnergy(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  if (iBody > 0)
    *dTmp = fdOrbPotEnergy(body,control,system,iBody);
  else
    *dTmp = -1;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergy(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergy(units,cUnit);
  }
}

void WriteTotOrbEnergy(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system, UNITS *units, UPDATE *update, int iBody, double *dTmp, char cUnit[]) {
  *dTmp = fdTotOrbEnergy(body,control,system);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergy(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergy(units,cUnit);
  }
}

void WriteTidalQ(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  // change to if eq
  // Just thermint, not eqtide
  if(body[iBody].bThermint)
    *dTmp = fdDynamicViscosity(body,iBody)*body[iBody].dMeanMotion/body[iBody].dShmodUMan;
  else
    *dTmp = body[iBody].dK2/body[iBody].dImK2;


  strcpy(cUnit,"");
}

void WriteImK2(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  // Just thermint, no eqtide
  if((body[iBody].bThermint && !body[iBody].bEqtide) || (body[iBody].bThermint && (!body[iBody].bOceanTides && body[iBody].bEnvTides)))
    *dTmp = fdImk2Man(body,iBody);
  else
    *dTmp = body[iBody].dImK2;

  strcpy(cUnit,"");
}

void WriteK2(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  // If just thermint
  if((body[iBody].bThermint && !body[iBody].bEqtide) || (body[iBody].bThermint && (!body[iBody].bOceanTides && !body[iBody].bEnvTides)))
    *dTmp = fdK2Man(body,iBody);
  else
    *dTmp = body[iBody].dK2;

  strcpy(cUnit,"");
}

void WriteXobl(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].dXobl;
  strcpy(cUnit,"");
}

void WriteYobl(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].dYobl;
  strcpy(cUnit,"");
}

void WriteZobl(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].dZobl;
  strcpy(cUnit,"");
}

void WriteMeanAnomaly(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {


  *dTmp = body[iBody].dMeanA;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle,cUnit);
  }
}

/*
 * End individual write functions
 */

void InitializeOutputGeneral(OUTPUT *output,fnWriteOutput fnWrite[]) {
  /*
   * Age
   */

  sprintf(output[OUT_AGE].cName,"Age");
  sprintf(output[OUT_AGE].cDescr,"System Age");
  sprintf(output[OUT_AGE].cNeg,"Gyr");
  output[OUT_AGE].bNeg = 1;
  output[OUT_AGE].dNeg = 1./(YEARSEC*1e9);
  output[OUT_AGE].iNum = 1;
  output[OUT_AGE].iModuleBit = 1;
  fnWrite[OUT_AGE] = &WriteAge;

  /*
   * BodyType
   */

  sprintf(output[OUT_BODYTYPE].cName,"BodyType");
  sprintf(output[OUT_BODYTYPE].cDescr,"Type of Body (0 == planet)");
  output[OUT_BODYTYPE].iNum = 1;
  output[OUT_BODYTYPE].iModuleBit = 1;
  fnWrite[OUT_BODYTYPE] = &WriteBodyType;

  /*
   * D
   */

  sprintf(output[OUT_DT].cName,"DeltaTime");
  sprintf(output[OUT_DT].cDescr,"Average Timestep Over Last Output Interval");
  sprintf(output[OUT_DT].cNeg,"years");
  output[OUT_DT].bNeg = 1;
  output[OUT_DT].dNeg = 1./YEARSEC;
  output[OUT_DT].iNum = 1;
  output[OUT_DT].iModuleBit = 1;
  fnWrite[OUT_DT] = &WriteDeltaTime;

  sprintf(output[OUT_DENSITY].cName,"Density");
  sprintf(output[OUT_DENSITY].cDescr,"Average Density");
  sprintf(output[OUT_DENSITY].cNeg,"solar");
  output[OUT_DENSITY].bNeg = 1;
  output[OUT_DENSITY].dNeg = 1./1420;  // kg/m^3
  output[OUT_DENSITY].iNum = 1;
  output[OUT_DENSITY].iModuleBit = 1;
  fnWrite[OUT_DENSITY] = &WriteDensity;

  /*
   * H
   */

  sprintf(output[OUT_HECC].cName,"HEcc");
  sprintf(output[OUT_HECC].cDescr,"Poincare's h (=e*sin(varpi)");
  output[OUT_HECC].bNeg = 0;
  output[OUT_HECC].iNum = 1;
  output[OUT_HECC].iModuleBit = EQTIDE + DISTORB;
  fnWrite[OUT_HECC] = &WriteHecc;

  sprintf(output[OUT_HZLIMITDRYRUNAWAY].cName,"HZLimitDryRunaway");
  sprintf(output[OUT_HZLIMITDRYRUNAWAY].cDescr,"Semi-major axis of Dry Runaway HZ Limit");
  output[OUT_HZLIMITDRYRUNAWAY].bNeg = 1;
  output[OUT_HZLIMITDRYRUNAWAY].dNeg = 1/AUCM;
  output[OUT_HZLIMITDRYRUNAWAY].iNum = 1;
  output[OUT_HZLIMITDRYRUNAWAY].iModuleBit = 1;
  fnWrite[OUT_HZLIMITDRYRUNAWAY] = &WriteHZLimitDryRunaway;

  /*
   * I
   */

  /* Imk2Man */
  sprintf(output[OUT_IMK2MAN].cName,"Imk2Man");
  sprintf(output[OUT_IMK2MAN].cDescr,"Imaginary Love Number k2 Mantle");
  sprintf(output[OUT_IMK2MAN].cNeg,"nd");
  output[OUT_IMK2MAN].bNeg = 1;
  output[OUT_IMK2MAN].dNeg = 1;
  output[OUT_IMK2MAN].iNum = 1;
  output[OUT_IMK2MAN].iModuleBit = THERMINT + EQTIDE; // XXX Is EQTIDE right?
  fnWrite[OUT_IMK2MAN] = &WriteImk2Man;

  /*
   * K
   */

  /* K2Man */
  sprintf(output[OUT_K2MAN].cName,"K2Man");
  sprintf(output[OUT_K2MAN].cDescr,"Real Love Number k2 Mantle");
  sprintf(output[OUT_K2MAN].cNeg,"nd");
  output[OUT_K2MAN].bNeg = 1;
  output[OUT_K2MAN].dNeg = 1;
  output[OUT_K2MAN].iNum = 1;
  output[OUT_K2MAN].iModuleBit = THERMINT + EQTIDE;
  fnWrite[OUT_K2MAN] = &WriteK2Man;

  sprintf(output[OUT_KECC].cName,"KEcc");
  sprintf(output[OUT_KECC].cDescr,"Poincare's k (=e*cos(varpi)");
  output[OUT_KECC].bNeg = 0;
  output[OUT_KECC].iNum = 1;
  output[OUT_KECC].iModuleBit = EQTIDE + DISTORB;
  fnWrite[OUT_KECC] = &WriteKecc;

  /*
   * L
   */

  sprintf(output[OUT_LONGP].cName,"LongP");
  sprintf(output[OUT_LONGP].cDescr,"Body's Longitude of pericenter");
  sprintf(output[OUT_LONGP].cNeg,"Deg");
  output[OUT_LONGP].bNeg = 1;
  output[OUT_LONGP].dNeg = 1./DEGRAD;
  output[OUT_LONGP].iNum = 1;
  output[OUT_LONGP].iModuleBit = EQTIDE + DISTORB + BINARY + SPINBODY;
  fnWrite[OUT_LONGP] = &WriteLongP;

  sprintf(output[OUT_ARGP].cName,"ArgP");
  sprintf(output[OUT_ARGP].cDescr,"Body's argument of pericenter");
  sprintf(output[OUT_ARGP].cNeg,"Deg");
  output[OUT_ARGP].bNeg = 1;
  output[OUT_ARGP].dNeg = 1./DEGRAD;
  output[OUT_ARGP].iNum = 1;
  output[OUT_ARGP].iModuleBit = DISTORB + GALHABIT + SPINBODY;
  fnWrite[OUT_ARGP] = &WriteBodyArgP;

  sprintf(output[OUT_LXUVTOT].cName,"LXUVTot");
  sprintf(output[OUT_LXUVTOT].cDescr,"Total XUV Luminosity");
  sprintf(output[OUT_LXUVTOT].cNeg,"LSUN");
  output[OUT_LXUVTOT].bNeg = 1;
  output[OUT_LXUVTOT].dNeg = 1./LSUN;
  output[OUT_LXUVTOT].iNum = 1;
  output[OUT_LXUVTOT].iModuleBit = STELLAR + ATMESC;
  fnWrite[OUT_LXUVTOT] = &WriteLXUVTot;

  /*
   * M
   */

  sprintf(output[OUT_MASS].cName,"Mass");
  sprintf(output[OUT_MASS].cDescr,"Mass");
  sprintf(output[OUT_MASS].cNeg,"Earth");
  output[OUT_MASS].bNeg = 1;
  output[OUT_MASS].dNeg = 1./MEARTH;
  output[OUT_MASS].iNum = 1;
  output[OUT_MASS].iModuleBit = 1;
  fnWrite[OUT_MASS] = &WriteMass;


  /*
   *   O
   */

  sprintf(output[OUT_OBL].cName,"Obliquity");
  sprintf(output[OUT_OBL].cDescr,"Obliquity");
  sprintf(output[OUT_OBL].cNeg,"deg");
  output[OUT_OBL].bNeg = 1;
  output[OUT_OBL].dNeg = DEGRAD;
  output[OUT_OBL].iNum = 1;
  output[OUT_OBL].iModuleBit = EQTIDE + DISTROT + POISE;
  fnWrite[OUT_OBL] = &WriteObliquity;

  sprintf(output[OUT_PRECA].cName,"PrecA");
  sprintf(output[OUT_PRECA].cDescr,"Body's precession angle");
  sprintf(output[OUT_PRECA].cNeg,"Deg");
  output[OUT_PRECA].bNeg = 1;
  output[OUT_PRECA].dNeg = 1./DEGRAD;
  output[OUT_PRECA].iNum = 1;
  output[OUT_PRECA].iModuleBit = EQTIDE + DISTROT + POISE;
  fnWrite[OUT_PRECA] = &WriteBodyPrecA;

  sprintf(output[OUT_ORBANGMOM].cName,"OrbAngMom");
  sprintf(output[OUT_ORBANGMOM].cDescr,"Orbital Angular Momentum");
  sprintf(output[OUT_ORBANGMOM].cNeg,"kgm^2/s");
  output[OUT_ORBANGMOM].bNeg = 1;
  output[OUT_ORBANGMOM].iNum = 1;
  output[OUT_ORBANGMOM].dNeg = 1.0;
  output[OUT_ORBANGMOM].iModuleBit = EQTIDE + DISTORB + BINARY + SPINBODY;
  fnWrite[OUT_ORBANGMOM] = &WriteOrbAngMom;

  sprintf(output[OUT_ORBECC].cName,"Eccentricity");
  sprintf(output[OUT_ORBECC].cDescr,"Orbital Eccentricity");
  output[OUT_ORBECC].iNum = 1;
  output[OUT_ORBECC].bNeg = 0;
  output[OUT_ORBECC].iModuleBit = EQTIDE + DISTORB + BINARY + GALHABIT + POISE + SPINBODY;
  fnWrite[OUT_ORBECC] = &WriteOrbEcc;

  sprintf(output[OUT_MEANA].cName,"MeanAnomaly");
  sprintf(output[OUT_MEANA].cDescr,"Orbital Mean Anomaly");
  sprintf(output[OUT_MEANA].cNeg,"Deg");
  output[OUT_MEANA].iNum = 1;
  output[OUT_MEANA].bNeg = 1;
  output[OUT_MEANA].dNeg = 1/DEGRAD;
  output[OUT_MEANA].iModuleBit = SPINBODY;
  fnWrite[OUT_MEANA] = &WriteMeanAnomaly;

  sprintf(output[OUT_ORBEN].cName,"OrbEnergy");
  sprintf(output[OUT_ORBEN].cDescr,"Body's Total Orbital Energy");
  sprintf(output[OUT_ORBEN].cNeg,"ergs");
  output[OUT_ORBEN].bNeg = 1;
  output[OUT_ORBEN].iNum = 1;
  output[OUT_ORBEN].iModuleBit = EQTIDE + DISTORB + BINARY;
  fnWrite[OUT_ORBEN] = &WriteOrbEnergy;

  sprintf(output[OUT_LOSTENG].cName,"LostEnergy");
  sprintf(output[OUT_LOSTENG].cDescr,"Body's Total Lost Energy");
  sprintf(output[OUT_LOSTENG].cNeg,"ergs");
  output[OUT_LOSTENG].bNeg = 1;
  output[OUT_LOSTENG].iNum = 1;
  output[OUT_LOSTENG].iModuleBit = EQTIDE + STELLAR;
  fnWrite[OUT_LOSTENG] = &WriteLostEng;

  sprintf(output[OUT_LOSTANGMOM].cName,"LostAngMom");
  sprintf(output[OUT_LOSTANGMOM].cDescr,"Lost Angular Momentum due to Magnetic Braking");
  sprintf(output[OUT_LOSTANGMOM].cNeg,"kgm^2/s");
  output[OUT_LOSTANGMOM].bNeg = 1;
  output[OUT_LOSTANGMOM].iNum = 1;
  output[OUT_LOSTANGMOM].dNeg = 1.0;
  output[OUT_LOSTANGMOM].iModuleBit = STELLAR;
  fnWrite[OUT_LOSTANGMOM] = &WriteLostAngMom;

  sprintf(output[OUT_ORBMEANMOTION].cName,"MeanMotion");
  sprintf(output[OUT_ORBMEANMOTION].cDescr,"Orbital Mean Motion");
  sprintf(output[OUT_ORBMEANMOTION].cNeg,"/day");
  output[OUT_ORBMEANMOTION].bNeg = 1;
  output[OUT_ORBMEANMOTION].dNeg = DAYSEC;
  output[OUT_ORBMEANMOTION].iNum = 1;
  output[OUT_ORBMEANMOTION].iModuleBit = EQTIDE + DISTORB + BINARY + SPINBODY;
  fnWrite[OUT_ORBMEANMOTION] = &WriteOrbMeanMotion;

  sprintf(output[OUT_ORBPER].cName,"OrbPeriod");
  sprintf(output[OUT_ORBPER].cDescr,"Orbital Period");
  sprintf(output[OUT_ORBPER].cNeg,"days");
  output[OUT_ORBPER].bNeg = 1;
  output[OUT_ORBPER].dNeg = 1./DAYSEC;
  output[OUT_ORBPER].iNum = 1;
  output[OUT_ORBPER].iModuleBit = EQTIDE + DISTORB + BINARY;
  fnWrite[OUT_ORBPER] = &WriteOrbPeriod;

  sprintf(output[OUT_ORBSEMI].cName,"SemiMajorAxis");
  sprintf(output[OUT_ORBSEMI].cDescr,"Semi-major Axis");
  sprintf(output[OUT_ORBSEMI].cNeg,"AU");
  output[OUT_ORBSEMI].bNeg = 1;
  output[OUT_ORBSEMI].dNeg = 1./AUCM;
  output[OUT_ORBSEMI].iNum = 1;
  output[OUT_ORBSEMI].iModuleBit = EQTIDE + DISTORB + BINARY + GALHABIT + SPINBODY;
  fnWrite[OUT_ORBSEMI] = &WriteOrbSemi;

  /*
   * R
   */

  sprintf(output[OUT_RADIUS].cName,"Radius");
  sprintf(output[OUT_RADIUS].cDescr,"Radius");
  output[OUT_RADIUS].bNeg = 1;
  sprintf(output[OUT_RADIUS].cNeg,"Solar");
  output[OUT_RADIUS].dNeg = 1./RSUN;
  output[OUT_RADIUS].iNum = 1;
  output[OUT_RADIUS].iModuleBit = 1;
  fnWrite[OUT_RADIUS] = &WriteRadius;

  sprintf(output[OUT_ROTANGMOM].cName,"RotAngMom");
  sprintf(output[OUT_ROTANGMOM].cDescr,"Rotational Angular Momentum");
  sprintf(output[OUT_ROTANGMOM].cNeg,"kgm^2/s");
  output[OUT_ROTANGMOM].bNeg = 1;
  output[OUT_ROTANGMOM].iNum = 1;
  output[OUT_ROTANGMOM].dNeg = 1.0;
  output[OUT_ROTANGMOM].iModuleBit = EQTIDE + DISTROT + STELLAR + BINARY;
  fnWrite[OUT_ROTANGMOM] = &WriteRotAngMom;

  sprintf(output[OUT_ROTKINENERGY].cName,"RotKinEnergy");
  sprintf(output[OUT_ROTKINENERGY].cDescr,"Body's Rotational Energy");
  sprintf(output[OUT_ROTKINENERGY].cNeg,"ergs");
  output[OUT_ROTKINENERGY].iNum = 1;
  output[OUT_ROTKINENERGY].iModuleBit = EQTIDE + DISTORB;
  fnWrite[OUT_ROTKINENERGY] = &WriteRotKinEnergy;

  sprintf(output[OUT_ROTPER].cName,"RotPer");
  sprintf(output[OUT_ROTPER].cDescr,"Rotational Period");
  sprintf(output[OUT_ROTPER].cNeg,"days");
  output[OUT_ROTPER].bNeg = 1;
  output[OUT_ROTPER].dNeg = 1./DAYSEC;
  output[OUT_ROTPER].iNum = 1;
  output[OUT_ROTPER].iModuleBit = EQTIDE + DISTROT + STELLAR;
  fnWrite[OUT_ROTPER] = &WriteRotPer;

  sprintf(output[OUT_ROTRATE].cName,"RotRate");
  sprintf(output[OUT_ROTRATE].cDescr,"Rotational Frequency");
  sprintf(output[OUT_ROTRATE].cNeg,"/day");
  output[OUT_ROTRATE].bNeg = 1;
  output[OUT_ROTRATE].dNeg = DAYSEC;
  output[OUT_ROTRATE].iNum = 1;
  output[OUT_ROTRATE].iModuleBit = EQTIDE + DISTROT + STELLAR;
  fnWrite[OUT_ROTRATE] = &WriteRotRate;

  sprintf(output[OUT_ROTVEL].cName,"RotVel");
  sprintf(output[OUT_ROTVEL].cDescr,"Rotational Velocity");
  sprintf(output[OUT_ROTVEL].cNeg,"km/s");
  output[OUT_ROTVEL].bNeg = 1;
  output[OUT_ROTVEL].dNeg = 1e-5;
  output[OUT_ROTVEL].iNum = 1;
  output[OUT_ROTVEL].iModuleBit = EQTIDE + DISTORB + STELLAR;
  fnWrite[OUT_ROTVEL] = &WriteRotVel;

  sprintf(output[OUT_SURFENFLUX].cName,"SurfEnFluxTotal");
  sprintf(output[OUT_SURFENFLUX].cDescr,"Total Surface Energy Flux");
  sprintf(output[OUT_SURFENFLUX].cNeg,"W/m^2");
  output[OUT_SURFENFLUX].bNeg = 1;
  output[OUT_SURFENFLUX].dNeg = 1;
  output[OUT_SURFENFLUX].iNum = 1;
  output[OUT_SURFENFLUX].iModuleBit = EQTIDE + RADHEAT + THERMINT;
  fnWrite[OUT_SURFENFLUX] = &WriteSurfaceEnergyFlux;

  sprintf(output[OUT_TIME].cName,"Time");
  sprintf(output[OUT_TIME].cDescr,"Simulation Time");
  sprintf(output[OUT_TIME].cNeg,"Gyr");
  output[OUT_TIME].bNeg = 1;
  output[OUT_TIME].dNeg = 1./(YEARSEC*1e9);
  output[OUT_TIME].iNum = 1;
  output[OUT_TIME].bGrid = 2;
  output[OUT_TIME].iModuleBit = 1;
  fnWrite[OUT_TIME] = &WriteTime;

  sprintf(output[OUT_TOTANGMOM].cName,"TotAngMom");
  sprintf(output[OUT_TOTANGMOM].cDescr,"Total Angular Momentum");
  sprintf(output[OUT_TOTANGMOM].cNeg,"kgm^2/s");
  output[OUT_TOTANGMOM].bNeg = 1;
  output[OUT_TOTANGMOM].iNum = 1;
  output[OUT_TOTANGMOM].dNeg = 1.0;
  output[OUT_TOTANGMOM].iModuleBit = EQTIDE + DISTORB + DISTROT + STELLAR + BINARY + SPINBODY;
  fnWrite[OUT_TOTANGMOM] = &WriteTotAngMom;

  sprintf(output[OUT_TOTENERGY].cName,"TotEnergy");
  sprintf(output[OUT_TOTENERGY].cDescr,"Total System Energy");
  sprintf(output[OUT_TOTENERGY].cNeg,"ergs");
  output[OUT_TOTENERGY].bNeg = 1;
  output[OUT_TOTENERGY].dNeg = 1;
  output[OUT_TOTENERGY].iNum = 1;
  output[OUT_TOTENERGY].iModuleBit = EQTIDE + DISTORB + STELLAR + SPINBODY;
  fnWrite[OUT_TOTENERGY] = &WriteTotEnergy;

  sprintf(output[OUT_POTENERGY].cName,"PotEnergy");
  sprintf(output[OUT_POTENERGY].cDescr,"Body's non-orbital Potential Energy");
  sprintf(output[OUT_POTENERGY].cNeg,"ergs");
  output[OUT_POTENERGY].bNeg = 1;
  output[OUT_POTENERGY].dNeg = 1;
  output[OUT_POTENERGY].iNum = 1;
  output[OUT_POTENERGY].iModuleBit = 1;
  fnWrite[OUT_POTENERGY] = &WritePotEnergy;

  sprintf(output[OUT_KINENERGY].cName,"KinEnergy");
  sprintf(output[OUT_KINENERGY].cDescr,"Body's non-orbital Kinetic Energy");
  sprintf(output[OUT_KINENERGY].cNeg,"ergs");
  output[OUT_KINENERGY].bNeg = 1;
  output[OUT_KINENERGY].dNeg = 1;
  output[OUT_KINENERGY].iNum = 1;
  output[OUT_KINENERGY].iModuleBit = 1;
  fnWrite[OUT_KINENERGY] = &WriteKinEnergy;

  sprintf(output[OUT_ORBKINENERGY].cName,"OrbKinEnergy");
  sprintf(output[OUT_ORBKINENERGY].cDescr,"Body's Orbital Kinetic Energy");
  sprintf(output[OUT_ORBKINENERGY].cNeg,"ergs");
  output[OUT_ORBKINENERGY].bNeg = 1;
  output[OUT_ORBKINENERGY].dNeg = 1;
  output[OUT_ORBKINENERGY].iNum = 1;
  output[OUT_ORBKINENERGY].iModuleBit = EQTIDE + DISTORB + BINARY + SPINBODY;
  fnWrite[OUT_ORBKINENERGY] = &WriteOrbKinEnergy;

  sprintf(output[OUT_ORBKINENERGY].cName,"TotOrbEnergy");
  sprintf(output[OUT_ORBKINENERGY].cDescr,"System's Total Orbital Energy");
  sprintf(output[OUT_ORBKINENERGY].cNeg,"ergs");
  output[OUT_ORBKINENERGY].bNeg = 1;
  output[OUT_ORBKINENERGY].dNeg = 1;
  output[OUT_ORBKINENERGY].iNum = 1;
  output[OUT_ORBKINENERGY].iModuleBit = EQTIDE + DISTORB + BINARY + SPINBODY;
  fnWrite[OUT_ORBKINENERGY] = &WriteTotOrbEnergy;

  sprintf(output[OUT_ORBPOTENERGY].cName,"OrbPotEnergy");
  sprintf(output[OUT_ORBPOTENERGY].cDescr,"Body's Orbital Potential Energy");
  sprintf(output[OUT_ORBPOTENERGY].cNeg,"ergs");
  output[OUT_ORBPOTENERGY].bNeg = 1;
  output[OUT_ORBPOTENERGY].dNeg = 1;
  output[OUT_ORBPOTENERGY].iNum = 1;
  output[OUT_ORBPOTENERGY].iModuleBit = EQTIDE + DISTORB + BINARY + SPINBODY;
  fnWrite[OUT_ORBPOTENERGY] = &WriteOrbPotEnergy;

  sprintf(output[OUT_IMK2].cName,"ImK2");
  sprintf(output[OUT_IMK2].cDescr,"Im(k_2)");
  output[OUT_IMK2].bNeg = 0;
  output[OUT_IMK2].iNum = 1;
  output[OUT_IMK2].iModuleBit = EQTIDE + THERMINT;
  fnWrite[OUT_IMK2] = &WriteImK2;

  sprintf(output[OUT_K2].cName,"K2");
  sprintf(output[OUT_K2].cDescr,"k_2");
  output[OUT_K2].bNeg = 0;
  output[OUT_K2].iNum = 1;
  output[OUT_K2].iModuleBit = EQTIDE + THERMINT;
  fnWrite[OUT_K2] = &WriteK2;

  sprintf(output[OUT_TIDALQ].cName,"TidalQ");
  sprintf(output[OUT_TIDALQ].cDescr,"Tidal Q");
  output[OUT_TIDALQ].bNeg = 0;
  output[OUT_TIDALQ].iNum = 1;
  output[OUT_TIDALQ].iModuleBit = EQTIDE + THERMINT;
  fnWrite[OUT_TIDALQ] = WriteTidalQ;

  sprintf(output[OUT_XOBL].cName,"Xobl");
  sprintf(output[OUT_XOBL].cDescr,"Body's sin(obl)*cos(pA)");
  output[OUT_XOBL].iNum = 1;
  output[OUT_XOBL].iModuleBit = EQTIDE + DISTROT;
  fnWrite[OUT_XOBL] = &WriteXobl;

  sprintf(output[OUT_YOBL].cName,"Yobl");
  sprintf(output[OUT_YOBL].cDescr,"Body's sin(obl)*sin(pA)");
  output[OUT_YOBL].iNum = 1;
  output[OUT_YOBL].iModuleBit = EQTIDE + DISTROT;
  fnWrite[OUT_YOBL] = &WriteYobl;

  sprintf(output[OUT_ZOBL].cName,"Zobl");
  sprintf(output[OUT_ZOBL].cDescr,"Body's cos(obl)");
  output[OUT_ZOBL].iNum = 1;
  output[OUT_ZOBL].iModuleBit = EQTIDE + DISTROT;
  fnWrite[OUT_ZOBL] = &WriteZobl;

}

// XXX MKS Units?
void CGSUnits(UNITS *units) {
  units->iTime = 0;
  units->iLength = 0;
  units->iMass = 0;
  units->iAngle = 0;
}

void WriteLogEntry(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite,FILE *fp,int iBody) {
  double *dTmp;
  char cUnit[48];
  UNITS units;
  int i,j,iNumIndices;


  cUnit[0]='\0';
  dTmp=malloc(output->iNum*sizeof(double));
  CGSUnits(&units);
  fnWrite(body,control,output,system,&units,update,iBody,dTmp,cUnit);

  fprintf(fp,"(%s) %s [%s]: ",output->cName,output->cDescr,cUnit);
  for (j=0;j<output->iNum;j++) {
    fprintd(fp,dTmp[j],control->Io.iSciNot,control->Io.iDigits);
    fprintf(fp," ");
  }
  free(dTmp);
  fprintf(fp,"\n");
}

void LogUnits(FILE *fp) {
  /* Mass Units */
  UNITS units;

  CGSUnits(&units);

  fprintf(fp,"Mass Units: ");
  if (units.iMass == 0) {
    fprintf(fp,"Kilograms\n");
  } else if (units.iMass == 1) {
    fprintf(fp,"Grams\n");
  } else if (units.iMass == 2) {
    fprintf(fp,"Solar\n");
  } else if (units.iMass == 3) {
    fprintf(fp,"Earth\n");
  } else if (units.iMass == 4) {
    fprintf(fp,"Jupiter\n");
  } else if (units.iMass == 5) {
    fprintf(fp,"Saturn\n");
  }

  /* Length Units */

  fprintf(fp,"Length Units: ");
  if (units.iLength == 0) {
    fprintf(fp,"Meters\n");
  } else if (units.iLength == 1) {
    fprintf(fp,"Centimeters\n");
  } else if (units.iLength == 2) {
    fprintf(fp,"Kilometers\n");
  } else if (units.iLength == 3) {
    fprintf(fp,"Solar\n");
  } else if (units.iLength == 4) {
    fprintf(fp,"Earth\n");
  } else if (units.iLength == 5) {
    fprintf(fp,"Jupiter\n");
  } else if (units.iLength == 6) {
    fprintf(fp,"AU\n");
  }

  /* Time Units */

  fprintf(fp,"Time Units: ");
  if (units.iTime == 0) {
    fprintf(fp,"Seconds\n");
  } else if (units.iTime == 1) {
    fprintf(fp,"Days\n");
  } else if (units.iTime == 2) {
    fprintf(fp,"Years\n");
  } else if (units.iTime == 3) {
    fprintf(fp,"Megayears\n");
  } else if (units.iTime == 4) {
    fprintf(fp,"Gigayears\n");
  }

  /* Angle Units */

  fprintf(fp,"Angle Units: ");
  if (units.iAngle == 0) {
    fprintf(fp,"Radians\n");
  } else if (units.iAngle == 1) {
    fprintf(fp,"Degrees\n");
  }

}

void LogIntegration(CONTROL *control,FILE *fp) {

  fprintf(fp,"Integration Method: ");
  if (control->Evolve.iOneStep == EULER)
    fprintf(fp,"Euler");
  else if (control->Evolve.iOneStep == RUNGEKUTTA)
    fprintf(fp,"Runge-Kutta4");
  fprintf(fp,"\n");

  fprintf(fp,"Direction: ");
  if (control->Evolve.bDoBackward)
    fprintf(fp,"Backward\n");
  else
    fprintf(fp,"Forward\n");

  fprintf(fp,"Time Step: ");
  fprintd(fp,control->Evolve.dTimeStep,control->Io.iSciNot,control->Io.iDigits);
  fprintf(fp,"\n");

  fprintf(fp,"Stop Time: ");
  fprintd(fp,control->Evolve.dStopTime,control->Io.iSciNot,control->Io.iDigits);
  fprintf(fp,"\n");

  fprintf(fp,"Output Interval: ");
  fprintd(fp,control->Io.dOutputTime,control->Io.iSciNot,control->Io.iDigits);
  fprintf(fp,"\n");

  fprintf(fp,"Use Variable Timestep: ");
  if (control->Evolve.bVarDt == 0) {
    fprintf(fp,"No\n");
  } else {
    fprintf(fp,"Yes\n");
    fprintf(fp,"dEta: ");
    fprintd(fp,control->Evolve.dEta,control->Io.iSciNot,control->Io.iDigits);
    fprintf(fp,"\n");
  }
}

void LogHalt(CONTROL *control,FILE *fp) {

  fprintf(fp,"Minimum Value of ecc and obl: ");
  fprintd(fp,control->Evolve.dMinValue,control->Io.iSciNot,control->Io.iDigits);
  fprintf(fp,"\n");
  fprintf(fp,"\n");

}

void LogBodyRelations(CONTROL *control,FILE *fp,int iBody) {

  if (control->iMassRad[iBody] > 0) {
    fprintf(fp,"Mass-Radius Relationship: ");
    if (control->iMassRad[iBody] == 1) {
      fprintf(fp,"New Light On Dark Stars\n");
    } else if (control->iMassRad[iBody] == 2) {
      fprintf(fp,"Gorda, S.Y. & Svechnikov, M.A. 1999, Astronomy Reports, 43, 521-525.\n");
    } else if (control->iMassRad[iBody] == 3) {
      fprintf(fp,"Bayless, A.J. & Orosz, J.A. 2006, ApJ, 651, 1155-1165.\n");
    } else if (control->iMassRad[iBody] == 4) {
      fprintf(fp,"Sotin, C. et al. 2007, Icarus, 191, 337-351.\n");
    } else {
      fprintf(fp,"Unknown!\n");
      fprintf(stderr,"Unknown Mass-Radius Relationship!\n");
      exit(EXIT_INPUT);
    }
  }
}

void LogOutputOrder(BODY *body,CONTROL *control,FILES *files,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp,int iBody) {
  int iCol,iOut,iSubOut,iExtra=0;
  char cCol[MODULEOUTEND][OUTLEN];
  double *dTmp;
  char cUnit[OUTLEN],cTmp[OUTLEN];

  for (iCol=0;iCol<files->Outfile[iBody].iNumCols;iCol++) {
    for (iOut=0;iOut<MODULEOUTEND;iOut++) {
      if (memcmp(files->Outfile[iBody].caCol[iCol],output[iOut].cName,strlen(output[iOut].cName)) == 0) {
        /* Match! */
        dTmp=malloc(output[iOut].iNum*sizeof(double));
        fnWrite[iOut](body,control,&output[iOut],system,&control->Units[iBody],update,iBody,dTmp,cUnit);
        for (iSubOut=0;iSubOut<output[iOut].iNum;iSubOut++) {
          strcpy(cCol[iCol+iSubOut+iExtra],files->Outfile[iBody].caCol[iCol]);
          sprintf(cTmp,"[%s]",cUnit);
          strcat(cCol[iCol+iSubOut+iExtra],cTmp);
        }
        iExtra += (output[iOut].iNum-1);
	free(dTmp);
      }

    }
  }

  fprintf(fp,"Output Order:");
  for (iCol=0;iCol<(files->Outfile[iBody].iNumCols + iExtra);iCol++)
    fprintf(fp," %s",cCol[iCol]);
  fprintf(fp, "\n");
}

void LogGridOutput(BODY *body,CONTROL *control,FILES *files,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp,int iBody) {
  int iCol,iOut,iSubOut,iExtra=0;
  char cCol[MODULEOUTEND][OUTLEN];
  double *dTmp;
  char cUnit[OUTLEN],cTmp[OUTLEN];

  for (iCol=0;iCol<files->Outfile[iBody].iNumGrid;iCol++) {
    for (iOut=0;iOut<MODULEOUTEND;iOut++) {
      if (memcmp(files->Outfile[iBody].caGrid[iCol],output[iOut].cName,strlen(output[iOut].cName)) == 0) {
        /* Match! */
        dTmp=malloc(output[iOut].iNum*sizeof(double));
        fnWrite[iOut](body,control,&output[iOut],system,&control->Units[iBody],update,iBody,dTmp,cUnit);
        for (iSubOut=0;iSubOut<output[iOut].iNum;iSubOut++) {
          strcpy(cCol[iCol+iSubOut+iExtra],files->Outfile[iBody].caGrid[iCol]);
          sprintf(cTmp,"[%s]",cUnit);
          strcat(cCol[iCol+iSubOut+iExtra],cTmp);
        }
        iExtra += (output[iOut].iNum-1);
  free(dTmp);
      }
    }
  }


  fprintf(fp,"Grid Output Order:");
  for (iCol=0;iCol<(files->Outfile[iBody].iNumGrid + iExtra);iCol++)
    fprintf(fp," %s",cCol[iCol]);
  fprintf(fp, "\n");
}

void LogOptions(CONTROL *control,FILES *files,MODULE *module,SYSTEM *system,FILE *fp) {
  int iFile,iModule;

  fprintf(fp,"-------- Log file %s -------\n\n",files->cLog);
  fprintf(fp,"Executable: %s\n",files->cExe);

  fprintf(fp,"System Name: %s\n",system->cName);
  fprintf(fp,"Primary Input File: %s\n",files->Infile[0].cIn);
  for (iFile=1;iFile<files->iNumInputs;iFile++)
    fprintf(fp,"Body File #%d: %s\n",iFile,files->Infile[iFile].cIn);
  fprintf(fp,"Allow files to be overwitten: ");
  if (control->Io.bOverwrite)
    fprintf(fp,"Yes");
  else
    fprintf(fp,"No");
  fprintf(fp,"\n");

  /* Log modules XXX?
  for (iModule=0;iModule<module->iNumModules;iModule++)
    module->fnLogOptions[iModule](control,fp);
  */

  LogUnits(fp);

  fprintf(fp,"\n------- FORMATTING -----\n");
  fprintf(fp,"Verbosity Level: %d\n",control->Io.iVerbose);
  fprintf(fp,"Crossover Decade for Scientific Notation: %d\n",control->Io.iSciNot);
  fprintf(fp,"Number of Digits After Decimal: %d\n",control->Io.iDigits);

  if (control->Evolve.bDoForward || control->Evolve.bDoBackward) {
    LogIntegration(control,fp);
    LogHalt(control,fp);
  }
}

void LogSystem(BODY *body,CONTROL *control,MODULE *module,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp) {
  int iOut,iModule;

  fprintf(fp,"SYSTEM PROPERTIES ----\n");

  for (iOut=OUTSTART;iOut<OUTBODYSTART;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,0);
  }

  /* Log modules? XXX
  for (iModule=0;iModule<module->iNumModules;iModule++)
    module->fnLog[iModule](control,output,body,system,update,fnWrite,fp);
 */
}

void LogBody(BODY *body,CONTROL *control,FILES *files,MODULE *module,OUTPUT *output,SYSTEM *system,fnWriteOutput fnWrite[],FILE *fp,UPDATE *update) {
  int iBody,iOut,iModule;

  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    fprintf(fp,"\n----- BODY: %s ----\n",body[iBody].cName);
    fprintf(fp,"Active Modules: ");
    PrintModuleList(fp,module->iBitSum[iBody]);
    fprintf(fp,"\n");
    fprintf(fp,"Module Bit Sum: %d\n",module->iBitSum[iBody]);
    fprintf(fp,"Color: %s\n", body[iBody].cColor);
    for (iOut=OUTBODYSTART;iOut<OUTEND;iOut++) {
      if (output[iOut].iNum > 0) {
	if (module->iBitSum[iBody] & output[iOut].iModuleBit) {
	  /* Useful for debugging
	  printf("%d %d\n",iBody,iOut);
	  fflush(stdout);
	  */
	  WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,iBody);
	}
      }
    }
    LogBodyRelations(control,fp,iBody);
    /* Log modules */
    for (iModule=0;iModule<module->iNumModules[iBody];iModule++)
      module->fnLogBody[iBody][iModule](body,control,output,system,update,fnWrite,fp,iBody);

    LogOutputOrder(body,control,files,output,system,update,fnWrite,fp,iBody);
    LogGridOutput(body,control,files,output,system,update,fnWrite,fp,iBody);
  }
}

void WriteLog(BODY *body,CONTROL *control,FILES *files,MODULE *module,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,fnWriteOutput fnWrite[],time_t dStartTime,int iEnd) {
  char cTime[OPTLEN];
  FILE *fp;
  double dDt,dTotTime;

  /* Get derivatives */
  PropertiesAuxiliary(body,control,update);
  dDt=fdGetUpdateInfo(body,control,system,update,fnUpdate);

  if (iEnd == 0) {
    sprintf(cTime,"Input");
    fp=fopen(files->cLog,"w");
    control->Evolve.dTime=0;
    control->Evolve.nSteps=0;
  } else if (iEnd == 1) {
    sprintf(cTime,"Final");
    fp=fopen(files->cLog,"a");
  } else if (iEnd == -1) {
    sprintf(cTime,"Initial");
    fp=fopen(files->cLog,"w");
  }

  if (!iEnd) {
    LogOptions(control,files,module,system,fp);

    fprintf(fp,"\n---- INITIAL ");
  } else
    fprintf(fp,"\n\n\n---- FINAL ");

  /* System Properties */
  LogSystem(body,control,module,output,system,update,fnWrite,fp);

  /* Bodies' Properties */
  LogBody(body,control,files,module,output,system,fnWrite,fp,update);

  if (iEnd) {
    dTotTime = difftime(time(NULL),dStartTime);
    fprintf(fp,"\nRuntime = %f s\n", dTotTime);
    fprintf(fp,"Total Number of Steps = %d\n",control->Evolve.nSteps);
    if (control->Io.iVerbose >= VERBPROG)
      printf("Runtime = %f s\n", dTotTime);
  }

  fclose(fp);
}

void WriteOutput(BODY *body,CONTROL *control,FILES *files,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput *fnWrite,double dTime,double dDt){
  int iBody,iCol,iOut,iSubOut,iExtra=0,iGrid,iLat,jBody,j;
  double dCol[NUMOPT],*dTmp,dGrid[NUMOPT];
  FILE *fp;
  char cUnit[OPTLEN], cPoiseGrid[NAMELEN], cLaplaceFunc[NAMELEN];

  /* Write out all data columns for each body. As some data may span more than
     1 column, we search the input list sequentially, adding iExtra to the
     total number of columns as we go. The calls to fnWrite return the column
     value in the correct units, and output.iNum already contains the
     number of columns. */

  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {

    //Need to get orbital elements for SpiNBody in case they're being output
    if (body[iBody].bSpiNBody) {
      Bary2OrbElems(body, iBody);
    }

    for (iCol=0;iCol<files->Outfile[iBody].iNumCols;iCol++) {
      for (iOut=0;iOut<MODULEOUTEND;iOut++) {
        if (output[iOut].bGrid == 0 || output[iOut].bGrid == 2) {
          if (memcmp(files->Outfile[iBody].caCol[iCol],output[iOut].cName,strlen(output[iOut].cName)) == 0) {
            /* Match! */
            dTmp=malloc(output[iOut].iNum*sizeof(double));
            fnWrite[iOut](body,control,&output[iOut],system,&control->Units[iBody],update,iBody,dTmp,cUnit);
            for (iSubOut=0;iSubOut<output[iOut].iNum;iSubOut++)
              dCol[iCol+iSubOut+iExtra]=dTmp[iSubOut];
            iExtra += (output[iOut].iNum-1);
	    free(dTmp);
	    dTmp = NULL;
          }
        }
      }
    }

    /* Now write the columns */
    if (files->Outfile[iBody].iNumCols > 0) {
      fp = fopen(files->Outfile[iBody].cOut,"a");
      for (iCol=0;iCol<files->Outfile[iBody].iNumCols+iExtra;iCol++) {
	fprintd(fp,dCol[iCol],control->Io.iSciNot,control->Io.iDigits);
	fprintf(fp," ");
      }
      fprintf(fp,"\n");
      fclose(fp);
    }
    // XXX Module-specific output should NOT be present in this file

    /* Grid outputs, currently only set up for POISE */
    if (body[iBody].bPoise) {
      dTmp = malloc(1*sizeof(double));
      for (iLat=0;iLat<body[iBody].iNumLats;iLat++) {
        for (iGrid=0;iGrid<files->Outfile[iBody].iNumGrid;iGrid++) {
          for (iOut=0;iOut<MODULEOUTEND;iOut++) {
            if (output[iOut].bGrid == 1 || output[iOut].bGrid == 2) {
              if (memcmp(files->Outfile[iBody].caGrid[iGrid],output[iOut].cName,strlen(output[iOut].cName)) == 0) {
                body[iBody].iWriteLat = iLat;
                fnWrite[iOut](body,control,&output[iOut],system,&control->Units[iBody],update,iBody,dTmp,cUnit);
                dGrid[iGrid]=*dTmp;
              }
            }
          }
        }
        /* Now write the columns */

        sprintf(cPoiseGrid,"%s.%s.Climate",system->cName,body[iBody].cName);

        if (control->Evolve.dTime == 0 && iLat == 0) {
          if (body[iBody].bClimateModel == SEA) {
            WriteDailyInsol(body,control,&output[iOut],system,&control->Units[iBody],update,iBody,dTmp,cUnit);
            WriteSeasonalTemp(body,control,&output[iOut],system,&control->Units[iBody],update,iBody,dTmp,cUnit);
            WriteSeasonalIceBalance(body,control,&output[iOut],system,&control->Units[iBody],update,iBody,dTmp,cUnit);
	    WriteSeasonalFluxes(body,control,&output[iOut],system,&control->Units[iBody],update,iBody,dTmp,cUnit);
	                WritePlanckB(body,control,&output[iOut],system,&control->Units[iBody],update,iBody,dTmp,cUnit);


            if (body[iBody].dSeasOutputTime != 0) {
              body[iBody].dSeasNextOutput = body[iBody].dSeasOutputTime;
            }
          }
          fp = fopen(cPoiseGrid,"w");
        } else {
          fp = fopen(cPoiseGrid,"a");
        }

        if (body[iBody].dSeasOutputTime != 0) {
          if (control->Evolve.dTime >= body[iBody].dSeasNextOutput && iLat == 0) {
            WriteDailyInsol(body,control,&output[iOut],system,&control->Units[iBody],update,iBody,dTmp,cUnit);
              WriteSeasonalTemp(body,control,&output[iOut],system,&control->Units[iBody],update,iBody,dTmp,cUnit);
              WriteSeasonalIceBalance(body,control,&output[iOut],system,&control->Units[iBody],update,iBody,dTmp,cUnit);
                          WriteSeasonalFluxes(body,control,&output[iOut],system,&control->Units[iBody],update,iBody,dTmp,cUnit);

            body[iBody].dSeasNextOutput = control->Evolve.dTime + body[iBody].dSeasOutputTime;
          }
        }

        for (iGrid=0;iGrid<files->Outfile[iBody].iNumGrid+iExtra;iGrid++) {
          fprintd(fp,dGrid[iGrid],control->Io.iSciNot,control->Io.iDigits);
          fprintf(fp," ");
        }
        fprintf(fp,"\n");
        fclose(fp);
      }
      free(dTmp);
    }
  }

  if (control->bOutputLapl) {
      for (iBody=1;iBody<(control->Evolve.iNumBodies-1);iBody++) {
        if (body[iBody].bDistOrb && body[iBody].bEqtide) {
          if (control->Evolve.iDistOrbModel == RD4) {
            /* open body file to write laplace functions and related */
            for (jBody=iBody+1;jBody<control->Evolve.iNumBodies;jBody++) {
              sprintf(cLaplaceFunc,"%s.%s.Laplace",body[iBody].cName,body[jBody].cName);
              if (control->Evolve.dTime == 0) {
                fp = fopen(cLaplaceFunc,"w");
              } else {
                fp = fopen(cLaplaceFunc,"a");
              }
              if (body[iBody].dSemi < body[jBody].dSemi) {
                for (j=0;j<LAPLNUM;j++) {
                  /* output alpha, laplace func, derivatives for each internal/external pair.
                  external/internal pairs are duplicates and so not output. this can create a
                  large amount of data for systems with lots of planets (78 columns/planet pair) */
                  fprintd(fp,system->dmAlpha0[system->imLaplaceN[iBody][jBody]][j], control->Io.iSciNot,control->Io.iDigits); //output alpha
                  fprintf(fp," ");

                  fprintd(fp,system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][j], control->Io.iSciNot,control->Io.iDigits); //output LaplaceC
                  fprintf(fp," ");

                  fprintd(fp,system->dmLaplaceD[system->imLaplaceN[iBody][jBody]][j], control->Io.iSciNot,control->Io.iDigits); //output LaplaceD
                  fprintf(fp," ");
                }
              }
              fprintf(fp,"\n");
              fclose(fp);
            }
          }
        } else if (body[iBody].bDistOrb) {
          if (control->Evolve.dTime == 0) {
            if (control->Evolve.iDistOrbModel == RD4) {
              /* open body file to write laplace functions and related */
              for (jBody=iBody+1;jBody<control->Evolve.iNumBodies;jBody++) {
                sprintf(cLaplaceFunc,"%s.%s.Laplace",body[iBody].cName,body[jBody].cName);
                fp = fopen(cLaplaceFunc,"w");
                if (body[iBody].dSemi < body[jBody].dSemi) {
                  for (j=0;j<LAPLNUM;j++) {
                    /* output alpha, laplace func, derivatives for each internal/external pair.
                    external/internal pairs are duplicates and so not output. this can create a
                    large amount of data for systems with lots of planets (78 columns/planet pair) */
                    fprintd(fp,system->dmAlpha0[system->imLaplaceN[iBody][jBody]][j], control->Io.iSciNot,control->Io.iDigits); //output alpha
                    fprintf(fp," ");

                    fprintd(fp,system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][j], control->Io.iSciNot,control->Io.iDigits); //output LaplaceC
                    fprintf(fp," ");

                    fprintd(fp,system->dmLaplaceD[system->imLaplaceN[iBody][jBody]][j], control->Io.iSciNot,control->Io.iDigits); //output LaplaceD
                    fprintf(fp," ");
                  }
                }
                fprintf(fp,"\n");
                fclose(fp);
              }
            }
          }
        }
      }
  }
}

void InitializeOutput(OUTPUT *output,fnWriteOutput fnWrite[]) {
  int iOut,iBody,iModule;

  for (iOut=0;iOut<MODULEOUTEND;iOut++) {
    sprintf(output[iOut].cName,"null");
    output[iOut].bGrid = 0;
    output[iOut].bNeg = 0; /* Is a negative option allowed */
    output[iOut].dNeg = 1; /* Conversion factor for negative options */
    output[iOut].iNum = 0; /* Number of parameters associated with option */
    output[iOut].bDoNeg = malloc(MAXBODIES*sizeof(int));
    for (iBody=0;iBody<MAXBODIES;iBody++)
        output[iOut].bDoNeg[iBody] = 0;
  }


  InitializeOutputGeneral(output,fnWrite);

  /* How can I do this without the developer needing to modify output.c??? I want the code to know about all possible output options so it can flag ouput parameters that do not apply to any uploaded module. XXXX*/

  /*
  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    output[iOut].bDoNeg[iBody] = 0;
    Initialize modules
    for (iModule=0;iModule<module->iNumModules[iBody];iModule++)
      module->fnInitializeOutput[iBody][iModule](output,fnWrite);
  }
  */

  /************************
   * ADD NEW MODULES HERE *
   ************************/

  InitializeOutputEqtide(output,fnWrite);
  InitializeOutputRadheat(output,fnWrite);
  InitializeOutputAtmEsc(output,fnWrite);
  InitializeOutputStellar(output,fnWrite);
  InitializeOutputDistOrb(output,fnWrite);
  InitializeOutputDistRot(output,fnWrite);
  InitializeOutputThermint(output,fnWrite);
  InitializeOutputPoise(output,fnWrite);
  InitializeOutputBinary(output,fnWrite);
  InitializeOutputFlare(output,fnWrite);
  InitializeOutputGalHabit(output,fnWrite);
  InitializeOutputSpiNBody(output, fnWrite);

}
