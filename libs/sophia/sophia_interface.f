INCLUDE "sophia_legacy.f"

c****************************************************************************
c
c   SOPHIAEVENT
c
c   interface between Sophia and CRPropa
c   simulate an interaction between p/n of given energy and the CMB
c
c   Eric Armengaud, 2005
c*******************************
c add Sept 2005 : redshift effect and interactions on IRB (from Primack 1999)
c****************************************************************************
c
c     Modified in Sept 2009 to include a radial dependence of IRB.
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      subroutine sophiaeventmod(nature,Ein,OutPart,OutPartType,NbOut
     &     Part,z_particle,bgFlag,Zmax_IRB,idatamax,en_data,flux_data)

c**********************************
c nature, Ein = input nature and energy of the nucleon
c        nature = 0 -> p ; 1 -> n
c        Ein : in GeV (SOPHIA standard energy unit)
c OutPart,OutPartType,NbOutPart = output data:
c        P(2000,5) list of 4-momenta + masses of output particles
c        LList(2000) list of output particle IDs
c        NP nb of output particles
c Added Sept. 2005 :
c        z_particle : needed to estimate the CMB temperature (no redshift 
c           evolution of IRB at the moment)
c        bgFlag = 1 for CMB, 2 for Primack et al. (1999) IRB
c Added Dec. 2005 :
c        zmax : now there is a "standard" IRB evolution which requires to 
c           know the redshift and z_max of the irb.
c**********************************

      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      IMPLICIT INTEGER (I-N)
      SAVE
      
      COMMON/input/ tbb,E0,alpha1,alpha2,
     &     epsm1,epsm2,epsb,L0

      COMMON /REDSHIFT/ Z, ZMAX_IR

      COMMON /S_PLIST/ P(2000,5), LLIST(2000), NP, Ideb
      COMMON /S_MASS1/ AM(49), AM2(49)
      COMMON /S_CHP/  S_LIFE(49), ICHP(49), ISTR(49), IBAR(49)
      COMMON /S_CSYDEC/ CBR(102), IDB(49), KDEC(612), LBARP(49)
      
      CHARACTER*6 NAMPRES
      COMMON /RES_PROP/ AMRES(9), SIG0(9),WIDTH(9), 
     +                    NAMPRES(0:9)

      CHARACTER*6 NAMPRESp
      COMMON /RES_PROPp/ AMRESp(9), BGAMMAp(9),WIDTHp(9),  
     +                    RATIOJp(9),NAMPRESp(0:9)

      CHARACTER*6 NAMPRESn
      COMMON /RES_PROPn/ AMRESn(9), BGAMMAn(9),WIDTHn(9),  
     +                    RATIOJn(9),NAMPRESn(0:9)


      external sample_eps,sample_s,eventgen,initial,prob_epskt,
     &     sample_ir_eps
      
      integer nature
      integer bgFlag
      double precision Ein,Pp
      double precision z_particle
      double precision OutPart(2000,5)
      integer OutPartType(2000)      
      integer NbOutPart

      double precision epsmin,epsmax

      DATA pi /3.141593D0/

cc 15.09.2009
      integer idatamax
      double precision en_data(idatamax),flux_data(idatamax) ! eV, eV/cm3
cc
      if (nature.eq.0) then 
         L0=13
      else if (nature.eq.1) then
         L0=14
      else
         print*,'sophiaevent: incoming particle incorrectly specified'
         stop
      endif
      
      print*,'sophiaeventmod version'

      call initial(L0)

      E0 = Ein
      pm = AM(L0)

      tbb=2.73*(1.D0+z_particle)
      Z = z_particle
      ZMAX_IR = Zmax_IRB

* check
c      return
***
      if (bgFlag.eq.1) then
         epsmin = 0.
         epsmax = 0.
         call sample_eps(epseV,epsmin,epsmax)
      else if (bgFlag.eq.2) then
c Choice for epsmin/epsmax : the Primack data is for -1<log(lambda/mumeter)<2.5
c  i.e. E in [3.93e-3,12.4] eV. We choose epsmin/max inside this range for security
         epsmin = 0.00395D0
         epsmax = 12.2D0
         call sample_ir_eps(epseV,epsmin,epsmax)
c     c 15.09.2009
c     Limits are defined by the provided background 
      else if (bgflag.eq.3) then
         epsmin = en_data(1) 
         epsmax = en_data(idatamax) 
         call sample_ir_eps2(epseV,epsmin,epsmax
     $        ,idatamax,en_data,flux_data)

cc

      else
         print*,'sophiaevent: incorrect background flag'
         stop
      endif
      eps = epseV/1.D9
c      Etot = E0+eps

c      print*,'Before sample_s'
      call sample_s(s,eps)
c      print*,'After sample_s'

c      gammap = E0/pm
c      betap = sqrt(1.D0-1.D0/gammap/gammap)
      Pp = sqrt(E0*E0-pm*pm)
      theta = ((pm*pm-s)/2.D0/eps+E0)/Pp
      if (theta.gt.1.D0) then
         print*,'sophiaevent: theta > 1.D0: ', theta
         theta = 0.D0
      else if (theta.lt.-1.D0) then
         print*,'sophiaevent: theta < -1.D0: ', theta
         theta = 180.D0
      else
          theta = acos(theta)*180.D0/pi 
      endif

      call eventgen(L0,E0,eps,theta,Imode)

      do i=1,2000
         do j=1,5
            OutPart(i,j)=P(i,j)
         end do
         OutPartType(i)=LLIST(i)
      end do
      NbOutPart=NP

      return
      end


ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

