# POSIX-Threads
Project for the course "Λειτουργικά Συστήματα" | CS - AUEB

## Διάγραμμα

![Theater Threads](https://github.com/paraskevasleivadaros/POSIX-Threads/blob/master/Theater%20%20-%20UML%20Diagram/Theater%20Threads.png)

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

## HOW TO RUN
Ανοίγεις το terminal και με την εντολή cd πηγαίνεις στο path του project
bash run.sh 

## TO-DO
Βάλε τις μεθόδους ακριβώς πριν την main!<br>
Δες τι παίζει με το cust_id>N_CUST

## Χρήσιμοι Σύνδεσμοι

[Λειτουργικά Συστήματα/Σύστημα Wiki/Ερωτήσεις για την 1η εργασία](https://eclass.aueb.gr/modules/wiki/page.php?course=INF168&wikiId=3941&action=show "Λειτουργικά Συστήματα/Σύστημα Wiki/Ερωτήσεις για την 1η εργασία") <br>
[How To Use Printf In Multiple Threads](https://stackoverflow.com/questions/23586682/how-to-use-printf-in-multiple-threads "How To Use Printf In Multiple Threads")

