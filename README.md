# ns-3-NB-IoT

## 1. Porject Overview 
Network simulator 3 (ns-3) is a popular open source network simulator, which supports multiple network scenarios, such as LTE. 

In order to explore the performance of NB-IoT, ns-3 NB-IoT module is required. Although the NB-IoT module is developing by official ns-3 team, some crucial part of NB-IoT havn't been involved yet, such as NB-IoT random access and NB-IoT scheduler. 

Therefore, in this project, we implement following part based on **ns-3.26 LTE module**.
* NB-IoT **Random Access(RA)** (SIB2-NB and CE level mechanism)
* NB-IoT **Schedulers** (SIB1-NB, NPDCCH, NPDSCH and NPUSCH) 

It is worth mentioning that this project only support format 1 RU. (15kHz, 12 SC and 2 slots)

## 2. Version
0.2
 
## 3. Contributor

Developed by Kuan-Yu Lin, Tzu-Hsian Chou and Ping-Han Shieh. (Graduate students in Wireless and Mobile Networking Labortory, National Tsing Hua University)

## 4. Prerequisite
The preequisite of our project is:
```
ns-3.26
```
The officaial download url of ns-3.26 is https://www.nsnam.org/releases/ns-3-26/.

We only test our project on:
```
Ubuntu 16.04
```


## 5. Build and Instalation
After the installation of ns-3.26, replace 

    ./<Your workplace>/ns-allinone-3.26/ns-3.26/src/lte/model
with the folder **"model"** we offered.

## 6. Implementation Detail
The implementation of **ns-3 NB-IoT module** is based on **3GPP NB-IoT release 13**. [1]-[7]
These references are offered in the **bottom of this article**.


The detail of our implementation is offered in follwing two articles:
[Simulating NB-IoT Random Access: Using ns-3 as an Example](https://drive.google.com/file/d/1RO6PNewPiJe7ZAkEgtAKxc5VUhM--pPT/view?usp=sharing)

[Simulating NB-IoT Scheduler: Using ns-3 as an Example](https://drive.google.com/file/d/1AOPB7SBYNIGs9xm2eaoXVam0Y6oW07nw/view?usp=sharing).

## 7. Getting Started
We offer two **example scripts** to explore the perfomance of NB-IoT RA and scheduler with our project, repectively.

The detail of **two example scripts** and **using limit of our project** are offered in following Gitub wiki urls:
* *NB-IoT Random Access*: https://github.com/a3794110/ns-3-NB-IoT/wiki/NB-IoT-RA-Example-Script
* *NB-IoT Schedulers*: https://github.com/a3794110/ns-3-NB-IoT/wiki/NB-IoT-Schedulers-Example-Script




## 9. Reference

[1] <sub>3GPP, “3rd Generation Partnership Project; Technical Specification Group Radio Access Network; Evolved Universal Terrestrial Radio Access (E-UTRA); Medium Access Control (MAC) protocol specification (Release 15) ,” Tech. Rep. 3G TS 36.321, 3GPP, March 2018.<sub>

[2]<sub> “ns-3 lte module design documentation.”<sub>

[3]<sub> 3GPP, “3rd Generation Partnership Project; Technical Specification Group Radio Access Network; Evolved Universal Terrestrial Radio Access (E-UTRA); Physical channels and modulation (Release 15),” Tech. Rep. 3G TS 36.211, 3GPP, March 2018.<sub>

[4]<sub>  3GPP, “3rd Generation Partnership Project; Technical Specification Group Radio Access Network; Evolved Universal Terrestrial Radio Access (E-UTRA); Requirements for support of radio resource management (Release 15),” Tech. Rep. 3G TS 36.133, 3GPP, March 2018 <sub>

[5]<sub> 3GPP, “3rd Generation Partnership Project; Technical Specification Group Radio Access Network; Evolved Universal Terrestrial Radio Access (E-UTRA); Radio Resource Control (RRC); Protocol specification (Release 13),” Tech. Rep. 3G TS 36.331, 3GPP, April 2018.<sub>


[6] <sub>3GPP, “3rd Generation Partnership Project; Technical Specification
Group Radio Access Network; Evolved Universal Terrestrial Radio Access (E-UTRA); Physical layer procedures (Release 15) ,” Tech. Rep. 3G TS 36.213, 3GPP, March 2018.<sub>

[7] <sub>3GPP, “3rd Generation Partnership Project; Technical Specification Group GSM/EDGE Radio Access Network; Cellular system support for ultra-low complexity and low throughput Internet of Things (CIoT) (Release 13) ,” Tech. Rep. 3G TR 45.820, 3GPP, November 2015.<sub>
