# POSIX-Threads
Project for the course "Λειτουργικά Συστήματα" | CS - AUEB<br>
Λειβαδάρος Παρασκευάς | 3150090 - Γιάννης Τριάντος | 3150173 - Στάθης Μπέτσης | 3120120

## How to Run
cd path (το path του project)<br>
bash run.sh 

# Περιγραφή Δομής Κώδικα

## Το αρχείο κεφαλίδας είναι το p3150173-p3150090-p3120120-res1.h

**Γραμμές 5-6 & 23:**<br>
&nbsp;&nbsp;&nbsp;&nbsp;#ifndef P3150173_P3150090_P3120120_RES1_H<br>
&nbsp;&nbsp;&nbsp;&nbsp;#define P3150173_P3150090_P3120120_RES1_H<br>
&nbsp;&nbsp;&nbsp;&nbsp;#endif<br>
Έχουμε ένα #include guard. Αυτό αποτρέπει τη διπλή δήλωση του αρχείου κεφαλίδας σε περίπτωση που ο κώδικας συμπεριληφθεί ξανά.<br>

**Γραμμές 8-12:<br>
&nbsp;&nbsp;&nbsp;&nbsp;#include <stdio.h><br>
&nbsp;&nbsp;&nbsp;&nbsp;#include <unistd.h><br>
&nbsp;&nbsp;&nbsp;&nbsp;#include <stdlib.h><br>
&nbsp;&nbsp;&nbsp;&nbsp;#include <pthread.h><br>
&nbsp;&nbsp;&nbsp;&nbsp;#include <stdbool.h>**<br>
Ενσωματώνουμε όλα τα αρχεία κεφαλίδας που χρειαζόμαστε στο .c αρχείο μας. Πιο συγκεκριμένα, το stdio.h περιέχει την δήλωση και την υλοποίηση της printf(). Το unistd.h παρέχει πρόσβαση στο API του POSIX. Το stdlib.h περιέχει την δήλωση και την υλοποίηση των
exit(), rand_r(), atoi(), exit() και του macro RAND_MAX. To pthread.h περιέχει την δήλωση και την υλοποίηση των pthread_mutex_init(), pthread_cond_init(), pthread_create(), pthread_join(), pthread_mutex_lock(), pthread_mutex_unlock(), pthread_cond_wait(), pthread_cond_broadcast(), pthread_mutex_destroy(), pthread_exit(), clock_gettime() και του macro CLOCK_REALTIME. Το stdbool.h περιέχει την δήλωση και την υλοποίηση του macro bool για τον τύπο δεδομένων boolean.<br>

**Γραμμές 14-21:**<br>
&nbsp;&nbsp;&nbsp;&nbsp;#define N_SEATS 250<br>
&nbsp;&nbsp;&nbsp;&nbsp;#define N_TEL 8<br>
&nbsp;&nbsp;&nbsp;&nbsp;#define N_SEAT_LOW 1<br>
&nbsp;&nbsp;&nbsp;&nbsp;#define N_SEAT_HIGH 5<br>
&nbsp;&nbsp;&nbsp;&nbsp;#define T_SEAT_LOW 5<br>
&nbsp;&nbsp;&nbsp;&nbsp;#define T_SEAT_HIGH 10<br>
&nbsp;&nbsp;&nbsp;&nbsp;#define P_CARD_SUCCESS 0.<br>
&nbsp;&nbsp;&nbsp;&nbsp;#define C_SEAT 20<br>
Ορίζουμε τις σταθερές που ζητούνται από την εκφώνηση.<br>


## Το αρχείο κώδικα C είναι το p3150173-p3150090-p3120120-res1.c<br>

**Γραμμή 5:**<br>
#include "p3150173-p3150090-p3120120-res1.h"<br>
Συμπεριλαμβάνουμε όλα τα περιεχόμενα του αρχείου p3150173-p3150090-p3120120-res1.h.<br>

**Γραμμές 7-69:**<br>
Δηλώνουμε όλες τις μεταβλητές και τις μεθόδους που θα χρησιμοποιήσουμε.<br>

**Γραμμές 71-126:**<br>
H main συνάρτηση του προγράμματος<br>
● **Γραμμές 73-87:** Ελέγχουμε τα ορίσματα που έχει δώσει ο χρήστης και αρχικοποιούμε τις μεταβλητές customer και seed.<br>
● **Γραμμές 91-92:** Ορίζουμε τον πίνακα threads που περιέχει στοιχεία τύπου pthread_t και τον πίνακα id που αποθηκεύουμε το αναγνωριστικό του κάθε πελάτη.<br>
● **Γραμμές 94-101:** Αρχικοποιούμε τα mutexes και ελέγχουμε εάν ήταν επιτυχείς όλες οι αρχικοποιήσεις με την check_rc().<br>
● **Γραμμές 103-114:**<br>
&nbsp;&nbsp;&nbsp;&nbsp;○ Στην γραμμή 103 ξεκινάμε την χρονομέτρηση με την startTimer().<br>
&nbsp;&nbsp;&nbsp;&nbsp;○ Στις γραμμές 105-108 ο επαναληπτικός βρόχος δημιουργεί τα threads δίνοντας τους σαν όρισμα τον πί<br>
&nbsp;&nbsp;&nbsp;&nbsp;○ Στις γραμμές 110-112 ο επαναληπτικός βρόχος συγχρονίζει όλα τα νήματα-παιδιά με το νήμα της main.<br>
&nbsp;&nbsp;&nbsp;&nbsp;○ Στην γραμμή 114 σταματάμε την χρονομέτρηση με την stopTimer().<br>
● **Γραμμές 116-123:** Καταστρέφουμε όλα τα νήματα και ελέγχουμε εάν καταστράφηκαν επιτυχώς.<br>
● **Γραμμή 125:** Καλούμε την συνάρτηση printInfo η οποία εμφανίζει όλα τα απαραίτητα στοιχεία της κράτησης.<br>

**Γραμμές 128-208:**<br> 
Η customer() είναι η start routine που χρησιμοποιούμε για τα νήματα. Παίρνει ένα όρισμα τύπου void pointer και επιστρέφει έναν void pointer.<br>
**● Γραμμή 130:** Δηλώνουμε και αρχικοποιούμε την μεταβλητή id για να αποθηκεύσουμε το αναγνωριστικό του κάθε πελάτη.<br>
**● Γραμμή 132:** Δηλώνουμε 4 struct τύπου timespec για να κρατάμε τις χρονικές στιγμές που αρχίζει και τελειώνει η αναμονή και η εξυπηρέτηση του κάθε πελάτη.<br>
**● Γραμμές 136-154:** Κλειδώνουμε τους τηλεφωνητές και ελέγχουμε αν υπάρχει κάποιος διαθέσιμος, αν ναι τους ξεκλειδώνουμε και συνεχίζουμε την εκτέλεση της customer, αν όχι περιμένουμε να λάβουμε σήμα για να συνεχίσουμε την εκτέλεση.<br>
**● Γραμμές 158-188:**<br>
&nbsp;&nbsp;&nbsp;&nbsp;**○** Στην γραμμή 158 ελέγχουμε εάν υπάρχουν διαθέσιμες θέσεις για κράτηση με την checkRemainingSeats(). Εάν δεν &nbsp;&nbsp;&nbsp;&nbsp;υπάρχουν εμφανίζεται στην κονσόλα πως το θέατρο γέμισε, το if αποτυγχάνει και η εκτέλεση μεταφέρεται στην γραμμή &nbsp;&nbsp;&nbsp;&nbsp;189. Εάν υπάρχουν η εκτέλεση μεταφέρεται στις γραμμές 159-187.<br>
&nbsp;&nbsp;&nbsp;&nbsp;**○** Στην γραμμή 160 καλούμε την choiceRandom() με ορίσματα τα N_SEAT_LOW και N_SEAT_HIGH για να &nbsp;&nbsp;&nbsp;&nbsp;αρχικοποιήσουμε την μεταβλητή seats που αναπαριστά τις θέσεις που επέλεξε ο πελάτης.<br>
&nbsp;&nbsp;&nbsp;&nbsp;**○** Στην γραμμή 162 καλούμε την sleep() με όρισμα sleepRandom(T_SEAT_LOW, T_SEAT_HIGH) γιατί η τηλεφωνήτρια &nbsp;&nbsp;&nbsp;&nbsp;θέλει κάποιο χρόνο για να βρει εάν υπάρχουν διαθέσιμες θέσεις.<br>
&nbsp;&nbsp;&nbsp;&nbsp;**○** Στην γραμμή 164 καλούμε checkAvailableSeats() με όρισμα το seats η οποία ελέγχει εάν υπάρχουν αρκετές &nbsp;&nbsp;&nbsp;&nbsp;θέσεις με βάση αυτές που ζήτησε ο πελάτης. Εάν υπάρχουν τότε ο έλεγχος περνάει στις γραμμές 165-186.<br>
&nbsp;&nbsp;&nbsp;&nbsp;**○** Στην γραμμή 166 κάνουμε κράτηση τις θέσεις με την bookSeats() η οποία παίρνει τα ορίσματα seats, id.<br>
&nbsp;&nbsp;&nbsp;&nbsp;**○** Στην γραμμή 168 καλούμε την POS() η οποία ελέγχει εάν η συναλλαγή με την κάρτα ήταν επιτυχής &nbsp;&nbsp;&nbsp;&nbsp;χρησιμοποιώντας την cardRandom(). Εάν ήταν τότε ο έλεγχος περνάει στις γραμμές 169-185. Εάν δεν ήταν επιτυχής η &nbsp;&nbsp;&nbsp;&nbsp;συναλλαγή με την κάρτα τότε αποδεσμεύουμε τις θέσεις της κράτησης και εμφανίζουμε στην κονσόλα πως η κράτηση &nbsp;&nbsp;&nbsp;&nbsp;ματαιώθηκε γιατί η συναλλαγή με την πιστωτική κάρτα δεν έγινε αποδεκτή.<br>
&nbsp;&nbsp;&nbsp;&nbsp;**○** Στις γραμμές 169-185 καταγράφουμε την συναλλαγή, τα έσοδα και εμφανίζεται στον πελάτη το αναγνωριστικό της &nbsp;&nbsp;&nbsp;&nbsp;συναλλαγής του, οι θέσεις του και το κόστος κάνοντας χρήση του logTransaction(), ενός επαναληπτικού βρόχου και &nbsp;&nbsp;&nbsp;&nbsp;της Cost() με όρισμα seats.<br>
**● Γραμμές 190-207:** Καταγράφουμε πως ο πελάτης εξυπηρετήθηκε και αποδεσμεύουμε την τηλεφωνήτρια ενημερώνοντας κιόλας τον μέσο χρόνο εξυπηρέτησης. Τέλος στην γραμμή 207 κάνουμε pthread_exit(NULL) για να αποδεσμεύσουμε το νήμα μας.<br>

**Γραμμές 210-221:**<br>
Υλοποιούμε την sleepRandom, την choiceRandom και την cardRandom οι οποίες επιστρέφουν από έναν τυχαίο αριθμό ο οποίος ανήκει σε ένα εύρος αντιστοίχως.

**Γραμμές 223-243:<br> 
● Γραμμές 223-249:** Η startTimer() αποθηκεύει στην μεταβλητή requestStart την τιμή του συστήματος του ρολογιού τη στιγμή που θα κληθεί.<br>
● **Γραμμές 231-237:** Η stopTimer() αποθηκεύει στην μεταβλητή requestEnd τιμή του συστήματος του ρολογιού τη στιγμή που θα κληθεί.<br>
● **Γραμμές 239-245:** Η Clock() εμφανίζει ένα ψηφιακό ρολόι στην κονσόλα.<br>

**Γραμμές 245-284:<br>
**● Γραμμές 245-251:** Η Cost() δέχεται ως όρισμα την μεταβλητή numOfSeats για να υπολογίσει το συνολικό ποσό που πρέπει να πληρώσει ο πελάτης. Αυξάνει την μεταβλητή profit με βάση το ποσό και επιστρέφει το ποσό.<br>
**● Γραμμές 253-258:** Η μέθοδος logTransaction() δεν παίρνει ορίσματα, ενημερώνει την μεταβλητή transactions αυξάνοντας την κατά 1 και επιστρέφει τον κωδικό (αναγνωριστικό) της πληρωμής.<br>
**● Γραμμές 260-271:** Η μέθοδος bookSeats() δέχεται ως όρισμα δύο μεταβλητές την numOfSeats και την custID. Δεσμεύει numOfSeats θέσεις στον πίνακα seatsPlan για το custID και μειώνει την μεταβλητή remainingSeatsPtr κατά numOfSeats.<br>
**● Γραμμές 273-284:** Η μέθοδος bookSeats() δέχεται ως όρισμα δύο μεταβλητές την numOfSeats και την custID. Διαγράφει όλες τις αλλαγές που έχει κάνει η bookSeats() για το ίδιο custID.<br>

**Γραμμές 286-295:**<br>
Η check_rc() δέχεται ως όρισμα μια μεταβλητή rc τύπου int και σε περίπτωση που η μεταβλητή rc ισούται με 1 εμφανίζει ένα μήνυμα σφάλματος και στην συνέχεια τερματίζει την εκτέλεση του προγράμματος με την exit(). Την χρησιμοποιούμε για να ελέγξουμε εάν η μέθοδοι
pthread_mutex_init(), pthread_cond_init(), pthread_create(), pthread_join(), pthread_mutex_lock(), pthread_mutex_unlock(), pthread_cond_wait(), pthread_cond_broadcast(), pthread_mutex_destroy() εκτελέστηκαν με επιτυχία.<br>

**Γραμμές 297-333:<br>
● Γραμμές 297-308:** Η checkAvailableSeats() ελέγχει εάν το πλήθος των θέσεων που ζήτησε να κλείσει ο πελάτης είναι μικρότερο ή ίσο από τις θέσεις που απομένουν ελεύθερες στο θέατρο. Επιστρέφει true ή false εάν ο έλεγχος αληθεύει ή όχι αντίστοιχα.<br>

**● Γραμμές 310-321:** Η συνάρτηση checkRemainingSeats() ελέγχει εάν υπάρχουν διαθέσιμες θέσεις στο θέατρο προς κράτηση. Επιστρέφει true ή false εάν ο έλεγχος αληθεύει ή όχι αντίστοιχα.<br>
**● Γραμμές 323-334:** Η συνάρτηση POS() ελέγχει εάν η κάρτα έγινε αποδεκτή ή όχι. Επιστρέφει true ή false εάν ο έλεγχος αληθεύει ή όχι αντίστοιχα.<br>
**Γραμμές 335-366:<br>
**● Γραμμές 335-346:** Η printDuration() εμφανίζει στην κονσόλα την διάρκεια εκτέλεσης όλων των παιδιών νημάτων της main.<br>
**● Γραμμές 348-364:** Η printSeatsPlan() εμφανίζει στην κονσόλα το πλάνο των θέσεων του θεάτρου με τον εξής τρόπο: σε κάθε γραμμή εμφανίζονται 4 θέσεις χωριζόμενες από κάθετες παύλες “|”.<br>
**● Γραμμές 366-384:** Η printInfo() εμφανίζει στην κονσόλα την Έναρξη και την Λήξη της χρονομέτρησης της εκτέλεσης όλων των νημάτων-παιδιών της main, καλεί την printDuration(), την printSeatsPlan() και στην συνέχεια τον μέσο χρόνο αναμονής, εξυπηρέτησης, το πλήθος των πελατών που εξυπηρετήθηκαν, το πλήθος των θέσεων που δεσμεύθηκαν, το πλήθος των θέσεων που έμειναν κενές, το πλήθος των συναλλαγών και το πλήθος των κερδών.<br>
