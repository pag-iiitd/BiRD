## BiRD : Race detector for binaries under relaxed memory models TSO and PSO

#### Prerequisites
```
Intel PIN (version higher than or equal to 2.14)
GCC (BiRD is built and evaluated on 4.8 version it may work on higher version)
Restricted by PIN's abilities, BiRD may not work on kernel 4.0 or higher.
BiRD is evaluated on Ubuntu 14.04, 18.04 and 20.04
```

#### Set up and Run
```
1. Set PIN_ROOT to the home directory of PIN or where ninary of PIN is located.
    export PIN_ROOT=/path/to/pin
2. Set PIN_ROOT to the home directory of PIN or where ninary of PIN is located.
    export PIN_ROOT=/path/to/pin
3. Set APP to client application
    export APP=/path/to/test
4. Set APP to client application
    export APP=/path/to/test   
5. Run ./schedule_random.sh    
```

#### Repository organization

```
DataRaceBench: Contains all tests used for evaluating BiRD.
Results: Traces generated by BiRD for tests in the DataRaceBench folder.
src: Source code for BiRD
```


Reset file backtract.out to empty and file endrun to "race" for every test.


[Reset path to endrun in schedule_random.sh]


Reset file backtract.out to empty and file endrun to "race" for every test.

Run ./schedule_random.sh


[Reset path to endrun in schedule_random.sh]



###### **Cite BiRD**

```
@article{jain2022bird,
  title={BiRD: Race Detection in Software Binaries under Relaxed Memory Models},
  author={Jain, Ridhi and Purandare, Rahul and Sharma, Subodh},
  journal={ACM Transactions on Software Engineering and Methodology},
  year={2022},
  publisher={ACM New York, NY}
}
```
