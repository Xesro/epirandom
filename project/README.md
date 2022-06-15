**Pour compiler les 4 niveaux:**
    _`make modules`_

Tous les niveau ont le même nom pour le device. Il faut donc remove un module pour en essayer un autre.

**Insertion d'un module:**
    _`sudo insmod epirandom_level_x.ko`_

**Ejection d'un module:**
    _`sudo rmmod epirandom_level_x`_

Les devices sont montés dans _dev/epirandom_

Pour le niveau 4 des paramètres sont présents:  
    _alphabetLength=6  
    alphabet='azerty'_  
ex : `insmod ./epirandom_level_4.ko alphabetLength=6 alphabet='azerty'_ `
