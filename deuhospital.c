#include <stdio.h>

#include <unistd.h>

#include <stdlib.h>

#include <pthread.h>

#include <semaphore.h>



#define PATIENT_NUMBER 1000

#define REGISTRATION_SIZE 10 //#The number of registration desks that are available.

#define RESTROOM_SIZE 10     //# The number of restrooms that are available.

#define CAFE_NUMBER 10       //# The number of cashiers in cafe that are available.

#define GP_NUMBER 10         //# The number of General Practitioner that are available.

#define PHARMACY_NUMBER 10   //# The number of cashiers in pharmacy that are available.

#define BLOOD_LAB_NUMBER 10  //# The number of assistants in blood lab that are available.

#define OR_NUMBER 10         //# The number of operating rooms, surgeons and nurses that are available.

#define SURGEON_NUMBER 30

#define NURSE_NUMBER 30



#define Waiting_for_registration 1

#define Waiting_for_gp 2

#define Waiting_for_pharmacy 3

#define Waiting_for_bloodlab 4

#define Waiting_for_surgery 5



void *people(void *peopleid);

void randwait(int secs);

void registration(int peopleid);

void waiting(int peopleid, int w, int re_examination);  //w means waiting for what. For restroom, cafeteria etc.

void restroom(int peopleid, int w, int re_examination); //re_examination for to hold information about a person's going to the general practitioner secondly(after blood lab/surgery)

void cafeteria(int peopleid, int w, int re_examination);

void general_practitioner(int peopleid, int re_examination);

void pharmacy(int peopleid);

void bloodlab(int peopleid);

void surgery(int peopleid);



//# The maximum number of surgeons and nurses that can do a surgery. A random value is calculated for each operation between 1 and given values to determine the required number of surgeons and nurses.

int SURGEON_LIMIT = 5;

int NURSE_LIMIT = 5;



//# The account of hospital where the money acquired from patients are stored.

int HOSPITAL_WALLET = 0;



int WAIT_TIME = 100;

int REGISTRATION_TIME = 100;

int GP_TIME = 200;

int PHARMACY_TIME = 100;

int BLOOD_LAB_TIME = 200;

int SURGERY_TIME = 500;

int CAFE_TIME = 100;

int RESTROOM_TIME = 100;



//# SURGERY OPERATION COST= SURGERY_OR_COST + (number of surgeons * SURGERY_SURGEON_COST) + (number of nurses * SURGERY_NURSE_COST)

int REGISTRATION_COST = 100;

int PHARMACY_COST = 200; //Calculated randomly between 1 and given value.

int BLOOD_LAB_COST = 200;

int SURGERY_OR_COST = 200;

int SURGERY_SURGEON_COST = 100;

int SURGERY_NURSE_COST = 50;

int CAFE_COST = 200; //200 Calculated randomly between 1 and given value.



//# The global increase rate of hunger and restroom needs of patients. It will increase randomly between 1 and given rate below.

int HUNGER_INCREASE_RATE = 10;

int RESTROOM_INCREASE_RATE = 10;

//The following patient properties needs to be created:

int Hunger_Meter = 100;   // Initialized between 1 and 100 at creation.

int Restroom_Meter = 100; // Initialized between 1 and 100 at creation



int peoples[PATIENT_NUMBER + 1];

int hunger[PATIENT_NUMBER + 1];

int bladder[PATIENT_NUMBER + 1];



sem_t registrate[REGISTRATION_SIZE + 1];

int registrationcounter = 0;



sem_t wc[RESTROOM_SIZE + 1];

int restroomcounter = 0;



sem_t cafe[CAFE_NUMBER + 1];

int cafecounter = 0;



sem_t gp[GP_NUMBER + 1];

int gpcounter = 0;



sem_t ph[PHARMACY_NUMBER + 1];

int phcounter = 0;



sem_t bl[BLOOD_LAB_NUMBER + 1];

int blcounter = 0;



sem_t s[OR_NUMBER + 1];

int scounter = 0;



sem_t surgeons[SURGEON_NUMBER + 1];

sem_t nurses[NURSE_NUMBER + 1];



int main(int argc, char *argv[])

{

    pthread_t pid[PATIENT_NUMBER + 1];



    for (int i = 1; i <= PATIENT_NUMBER; i++)

        peoples[i] = i;



    for (int i = 1; i <= PATIENT_NUMBER; i++)

        hunger[i] = (rand() % Hunger_Meter) + 1;



    for (int i = 1; i <= PATIENT_NUMBER; i++)

        bladder[i] = (rand() % Restroom_Meter) + 1;



    for (int i = 0; i < REGISTRATION_SIZE; i++)

        sem_init(&registrate[i], 0, 1);



    for (int i = 0; i < RESTROOM_SIZE; i++)

        sem_init(&wc[i], 0, 1);



    for (int i = 0; i < CAFE_NUMBER; i++)

        sem_init(&cafe[i], 0, 1);



    for (int i = 0; i < GP_NUMBER; i++)

        sem_init(&gp[i], 0, 1);



    for (int i = 0; i < PHARMACY_NUMBER; i++)

        sem_init(&ph[i], 0, 1);



    for (int i = 0; i < BLOOD_LAB_NUMBER; i++)

        sem_init(&bl[i], 0, 1);



    for (int i = 0; i < OR_NUMBER; i++)

        sem_init(&s[i], 0, 1);



    for (int i = 1; i <= SURGEON_NUMBER; i++)

        sem_init(&surgeons[i], 0, 1);



    for (int i = 1; i <= NURSE_NUMBER; i++)

        sem_init(&nurses[i], 0, 1);



    // Create the peoples.

    for (int i = 1; i <= PATIENT_NUMBER; i++)

    {

        pthread_create(&pid[i], NULL, people, (void *)&peoples[i]); //Peoples' disease generated in the general_practitoner function.

        randwait(WAIT_TIME);                                        //Patients comes to the hospital with random time between 0-100 milliseconds.

    }



    // Join each of the threads to wait for them to finish.

    for (int i = 1; i <= PATIENT_NUMBER; i++)

        pthread_join(pid[i], NULL);



    printf("Day is end. Hospital wallet is %d \n", HOSPITAL_WALLET);



    return 0;

}



void *people(void *peopleid)

{

    int pid = *(int *)peopleid;

    printf("People %d arrived at hospital.\n", pid);

    registration(pid); //Firstly people goes to the registration.

}



void randwait(int secs)

{

    // random wait time

    int rwt = rand() % secs;

    float random_wait_time = (float)rwt / 100;

    sleep(random_wait_time);

}



void registration(int peopleid)

{



    if (sem_trywait(&registrate[registrationcounter]) == 0)

    {

        int deskno = registrationcounter;

        registrationcounter++;

        registrationcounter = registrationcounter % REGISTRATION_SIZE;

        printf("People %d arrived at registration deck %d.\n", peopleid, deskno + 1);

        randwait(REGISTRATION_TIME);

        HOSPITAL_WALLET += REGISTRATION_COST;

        printf("People %d 's registration is done, leaves registration deck %d.\n", peopleid, deskno + 1);

        sem_post(&registrate[deskno]);

        general_practitioner(peopleid, 0); //when registration is done, people goes to the general practitioner.

    }

    else

    {

        printf("People %d is waiting for registration.\n", peopleid);

        waiting(peopleid, Waiting_for_registration, 0);

    }

}



void waiting(int peopleid, int w, int re_examination) //w is waiting for what. For registration or cafe etc.

{

    randwait(WAIT_TIME);

    hunger[peopleid] += (rand() % HUNGER_INCREASE_RATE) + 1;

    bladder[peopleid] += (rand() % RESTROOM_INCREASE_RATE) + 1;

    if (bladder[peopleid] > 99)

    {

        printf("People %d is going to the restroom. \n", peopleid);

        restroom(peopleid, w, re_examination);

    }

    else if (hunger[peopleid] > 99)

    {

        printf("People %d is going to the cafeteria. \n", peopleid);

        cafeteria(peopleid, w, re_examination);

    }

    else

    {

        if (w == 1)

        {

            printf("People %d is waiting for registration. \n", peopleid);

            registration(peopleid);

        }

        else if (w == 2)

        {

            printf("People %d is waiting general practitioner. \n", peopleid);

            general_practitioner(peopleid, re_examination);

        }

        else if (w == 3)

        {

            printf("People %d is waiting for pharmacy. \n", peopleid);

            pharmacy(peopleid);

        }

        else if (w == 4)

        {

            printf("People %d is waiting for blood lab. \n", peopleid);

            bloodlab(peopleid);

        }

        else if (w == 5)

        {

            printf("People %d is waiting for surgery operation. \n", peopleid);

            surgery(peopleid);

        }

    }

}



void restroom(int peopleid, int w, int re_examination)

{

    if (sem_trywait(&wc[restroomcounter]) == 0)

    {

        int restroomno = restroomcounter;

        restroomcounter++;

        restroomcounter = restroomcounter % RESTROOM_SIZE;

        printf("People %d arrived at restroom %d.\n", peopleid, restroomno + 1);

        randwait(RESTROOM_TIME);

        bladder[peopleid] = 0;

        printf("People %d's restroom need is done, leaves restroom %d.\n", peopleid, restroomno + 1);

        sem_post(&wc[restroomno]);

        waiting(peopleid, w, re_examination);

    }

    else

    {

        printf("People %d is waiting for restroom.\n", peopleid);

        waiting(peopleid, w, re_examination);

    }

}



void cafeteria(int peopleid, int w, int re_examination)

{

    if (sem_trywait(&cafe[cafecounter]) == 0)

    {

        int cafeno = cafecounter;

        cafecounter++;

        cafecounter = cafecounter % CAFE_NUMBER;

        printf("People %d arrived at cafeteria %d.\n", peopleid, cafeno + 1);

        randwait(CAFE_TIME);

        int cafe_spending = 0;

        cafe_spending = (rand() % CAFE_COST) + 1;

        HOSPITAL_WALLET += cafe_spending;

        hunger[peopleid] = 0;

        printf("People %d's cafeteria need is done, cafe spending is %d, leaves cafeteria %d.\n", peopleid, cafe_spending, cafeno + 1);

        sem_post(&cafe[cafeno]);

        waiting(peopleid, w, re_examination);

    }

    else

    {

        printf("People %d is waiting for cafeteria.\n", peopleid);

        waiting(peopleid, w, re_examination);

    }

}



void general_practitioner(int peopleid, int re_examination)

{

    if (sem_trywait(&gp[gpcounter]) == 0)

    {

        int gpno = gpcounter;

        gpcounter++;

        gpcounter = gpcounter % GP_NUMBER;

        printf("People %d arrived at genereal practitoner office %d.\n", peopleid, gpno + 1);

        randwait(GP_TIME);

        if (re_examination == 0) //People  first time coming to the general practitioner

        {

            int er = rand() % 3; // er means examination result. 0-> go to pharmacy, 1-> go to blood lab, 2-surgery operation

            if (er == 0)

            {

                printf("People %d is examined by general practitioner %d, examination result is taking medicine. People %d is going to the pharmacy.\n", peopleid, gpno + 1, peopleid);

                sem_post(&gp[gpno]);

                pharmacy(peopleid);

            }

            else if (er == 1)

            {

                printf("People %d is examined by general practitioner %d, examination result is blood analysis. People %d is going to the blood lab.\n", peopleid, gpno + 1, peopleid);

                sem_post(&gp[gpno]);

                bloodlab(peopleid);

            }

            else

            {

                printf("People %d is examined by general practitioner %d, examination result is surgery operation. People %d is going to the operating room.\n", peopleid, gpno + 1, peopleid);

                sem_post(&gp[gpno]);

                surgery(peopleid);

            }

        }

        else if (re_examination == 1)

        {                        //People comes general practitioner office after blood lab to examine blood results.

            int er = rand() % 2; // er means examination result. 0-> go to pharmacy, 1-> go to home.

            if (er == 0)

            {

                printf("People %d re-examined by general practitioner %d after the blood results, examination result is taking medicine. People %d is going to the pharmacy.\n", peopleid, gpno + 1, peopleid);

                sem_post(&gp[gpno]);

                pharmacy(peopleid);

            }

            else

            {

                printf("People %d is re-examined by general practitioner %d after the blood results,People %d is healthy, can go to home.\n", peopleid, gpno + 1, peopleid);

                sem_post(&gp[gpno]);

            }

        }

        else

        {                        //People comes general practitioner office after surgery to re-examine.

            int er = rand() % 2; // er means examination result. 0-> go to pharmacy, 1-> go to home.

            if (er == 0)

            {

                printf("People %d re-examined by general practitioner %d after surgery operation, examination result is taking medicine. People %d is going to the pharmacy.\n", peopleid, gpno + 1, peopleid);

                sem_post(&gp[gpno]);

                pharmacy(peopleid);

            }

            else

            {

                printf("People %d is re-examined by general practitioner %d after the surgery operation,People %d is healthy, can go to home.\n", peopleid, gpno + 1, peopleid);

                sem_post(&gp[gpno]);

            }

        }

    }

    else

    {

        printf("People %d is waiting general practitioner.\n", peopleid);

        waiting(peopleid, Waiting_for_gp, re_examination);

    }

}



void pharmacy(int peopleid)

{

    if (sem_trywait(&ph[phcounter]) == 0)

    {

        int phno = phcounter;

        phcounter++;

        phcounter = phcounter % PHARMACY_NUMBER;

        printf("People %d arrived at pharmacy %d.\n", peopleid, phno + 1);

        randwait(PHARMACY_TIME);

        int phcost = (rand() % PHARMACY_COST) + 1;

        HOSPITAL_WALLET += phcost;

        printf("People %d buy medicines, pharmacy cost is %d,leaves pharmacy %d and goes to the home.\n", peopleid, phcost, phno + 1);

        sem_post(&ph[phno]);

    }

    else

    {

        printf("People %d is waiting pharmacy.\n", peopleid);

        waiting(peopleid, Waiting_for_pharmacy, 0);

    }

}



void bloodlab(int peopleid)

{

    if (sem_trywait(&bl[blcounter]) == 0)

    {

        int blno = blcounter;

        blcounter++;

        blcounter = blcounter % BLOOD_LAB_NUMBER;

        printf("People %d arrived at blood lab %d.\n", peopleid, blno + 1);

        randwait(BLOOD_LAB_TIME);

        HOSPITAL_WALLET += BLOOD_LAB_COST;

        printf("People %d give blood, leaves blood lab %d and goes to the general practitioner office to re-examine.\n", peopleid, blno + 1);

        sem_post(&bl[blno]);

        general_practitioner(peopleid, 1);

    }

    else

    {

        printf("People %d is waiting blood lab.\n", peopleid);

        waiting(peopleid, Waiting_for_bloodlab, 1);

    }

}



void surgery(int peopleid)

{

    if (sem_trywait(&s[scounter]) == 0)

    {

        int sno = scounter;

        int needed_surgeon = (rand() % SURGEON_LIMIT) + 1;

        int needed_nurse = (rand() % NURSE_LIMIT) + 1;

        int ns = needed_surgeon;

        int nn = needed_nurse;

        int opcost = SURGERY_OR_COST + needed_surgeon * SURGERY_SURGEON_COST + needed_nurse * SURGERY_NURSE_COST;

        if (needed_surgeon > 1 && needed_nurse > 1)

            printf("People %d arrived at operating room %d. People %d is waiting %d surgeons and %d nurses.\n", peopleid, sno + 1, peopleid, needed_surgeon, needed_nurse);

        else if (needed_surgeon > 1 && needed_nurse == 1)

            printf("People %d arrived at operating room %d. People %d is waiting %d surgeons and 1 nurse.\n", peopleid, sno + 1, peopleid, needed_surgeon);

        else if (needed_surgeon == 1 && needed_nurse > 1)

            printf("People %d arrived at operating room %d. People %d is waiting 1 surgeon and %d nurse.\n", peopleid, sno + 1, peopleid, needed_nurse);

        else

            printf("People %d arrived at operating room %d. People %d is waiting 1 surgeon and 1 nurse.\n", peopleid, sno + 1, peopleid);

        for (int i = 1; i <= SURGEON_NUMBER; i++)

        {

            if (ns > 0)

            {

                if (sem_trywait(&surgeons[i]) == 0)

                {

                    printf("Surgeon %d is preparing for people %d's operation at operation room %d.\n", i, peopleid, sno + 1);

                    ns--;

                    if (ns == 0)

                        printf("People %d is waiting %d nurses at operation room %d.\n", peopleid, nn, sno + 1);

                    else if (ns == 1)

                        printf("People %d is waiting 1 more surgeon and %d nurses at operation room %d.\n", peopleid, nn, sno + 1);

                    else

                        printf("People %d is waiting %d more surgeon and %d nurses at operation room %d.\n", peopleid, ns, nn, sno + 1);

                }

            }

        }



        for (int i = 1; i <= NURSE_NUMBER; i++)

        {

            if (nn > 0)

            {

                if (sem_trywait(&nurses[i]) == 0)

                {

                    printf("Nurse %d is preparing for people %d's operation at operation room %d.\n", i, peopleid, sno + 1);

                    nn--;

                    if (nn == 0)

                        printf("People %d's operation is starting at operation room %d with %d surgeons and %d nurses.\n", peopleid, sno + 1, needed_surgeon, needed_nurse);

                    else if (nn == 1)

                        printf("People %d is waiting 1 more nurse at operation room %d\n", peopleid, sno + 1);

                    else

                        printf("People %d is waiting %d more nurses more at operation room %d\n", peopleid, nn, sno + 1);

                }

            }

        }

        scounter++;

        scounter = scounter % OR_NUMBER;

        randwait(SURGERY_TIME);

        HOSPITAL_WALLET += opcost;

        int value;

        for (int i = 1; i <= SURGEON_NUMBER; i++)

        {

            if (ns != needed_surgeon)

            {

                sem_getvalue(&surgeons[i], &value);

                if (value == 0)

                {

                    sem_post(&surgeons[i]);

                    ns++;

                }

            }

        }

        for (int i = 1; i <= NURSE_NUMBER; i++)

        {

            if (nn != needed_nurse)

            {

                sem_getvalue(&nurses[i], &value);

                if (value == 0)

                {

                    sem_post(&nurses[i]);

                    nn++;

                }

            }

        }

        printf("People %d's surgery operation is done succesfully with %d surgeons and %d nurses,operation cost is %d. People %d leaves operaing room %d and goes to the general practitioner to re-examine.\n", peopleid, needed_surgeon, needed_nurse, opcost, peopleid, sno + 1);

        sem_post(&s[sno]);

        general_practitioner(peopleid, 2);

    }

    else

    {

        printf("People %d is waiting for surgery operation.\n", peopleid);

        waiting(peopleid, Waiting_for_surgery, 2);

    }

}