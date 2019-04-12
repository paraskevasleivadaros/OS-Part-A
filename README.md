# POSIX-Threads
Project for the course "Λειτουργικά Συστήματα" | CS - AUEB

## Σημειώσεις

1 νήμα για κάθε πελάτη <br>
1 νήμα για κάθε τηλεφωνητή

Δεν ξέρουμε ποιος θα πάρει πρώτος χρόνο CPU!

pthreads API, τα mutexes είναι κλειδαριές (duh)

Το δύσκολο είναι να εξετάσουμε τι μπορεί να συμβεί..

Μπορούν να τρέχουν 10 πελάτες και κανένας τηλεφωνητής για παράδειγμα

Υπάρχουν πολλά σημεία κρίσιμου κώδικα όπως το πόσοι τηλεφωνητές είναι διαθέσιμοι και το πόσες θέσεις είναι διαθέσιμες (κάνουμε lock)

Για παράδειγμα, αν ο κρίσιμος κώδικας είναι ο αριθμός των διαθέσιμων τηλεφωνητών. Έστω πως έχουμε έναν counter που τοποθετείται σε mutex αν πάει κάποιος και αφαιρέσει έναν τηλεφωνητή θα πρέπει να ενημερώσει τον counter αφού περάσει απ'το ίδιο mutex. 

Φτιάχνω ένα mutex ονόματι Χ, όταν θέλω να κάνω κάτι ξεκλειδώνω το Χ και ενημερώνω τις υπόλοιπες διεργασίες πως θα κάνω κάτι. Όταν τελειώσω τη δουλειά μου κλειδώνω το Χ!

## Ιδέες
Πίνακας N_CUST από pointers που δείχνουν σε arrays που περιέχουν τις θέσεις που έχει κλείσει ο κάθε πελάτης

## How to Run
Ανοίγεις το terminal και με την εντολή cd πηγαίνεις στο path του project<br>
bash run.sh 

## TO-DO
Αν θες να περάσεις δύο ορίσματα στην pthread create βάλε struct! (Θυμήσου να το κάνεις μετά cast σε struct!)!<br>
Το init στα mutex κάνε το πριν τα create στην main!!<br>
Αντί για signal χρησιμοποίησε το broadcast!<br>
Το for σου θα είναι για τους πελάτες!<br>
το sleep όχι μέσα σε lock unlock!<br>
free στο τέλος για την malloc!<br>
Αντί για N_TEL βάλε  N_CUST!<br>
check number of arguments!<br>
check παντού το rc!<br>
1 condition!<br>
7 mutexes!<br>

## Χρήσιμοι Σύνδεσμοι

[Λειτουργικά Συστήματα/Σύστημα Wiki/Ερωτήσεις για την 1η εργασία](https://eclass.aueb.gr/modules/wiki/page.php?course=INF168&wikiId=3941&action=show "Λειτουργικά Συστήματα/Σύστημα Wiki/Ερωτήσεις για την 1η εργασία") <br>
[How To Use Printf In Multiple Threads](https://stackoverflow.com/questions/23586682/how-to-use-printf-in-multiple-threads "How To Use Printf In Multiple Threads") <br>
[How to use Mutex in multi-threaded coding](https://youtu.be/EthJVl3G21Y "How to use Mutex in multi-threaded coding")<br>
