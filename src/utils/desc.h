/**
 * Calcule et ajoute à une passe les notes de la variation 1 ascendante
 */
void addPassNoteVar1DESC(Pass * pass, Melody * melody) {
  for (int i = melody->inputLength - 1; i >= 0 ; i--) {
    int degree = melody->getInputAt(i)->degree;
    pass->addNote( degree, melody->octaveLength - pass->rank + 1 );
  }
}

/**
 * Calcule et ajoute à une passe les notes de la variation 1 ascendante
 */
void addPassNoteVar23DESC(Pass * pass, Melody * melody) {
  int inputRanks[4];

  // Index à partir du quel on compte les 4 rang composant la pass
  int startAt = (melody->inputLength < 5 || pass->rank % 2 == 0) 
    ? melody->inputLength - 1 
    : 3;

  // On attributs les rangs dans le bon ordre
  for (int i=3; i >=0; i--) {
    inputRanks[i] = melody->getInputAt(startAt - i)->degree;
  }

  // Calcule de l'octave courant
  int octave = (melody->inputLength <= 4)
    ? melody->octaveLength - pass->rank + 1 
    : melody->octaveLength - (pass->rank / 2) + 1;

  // Quel rangs on inverse avec le rang 0
  int switchRank = pass->variation == 2 ? 1 : 2;

  for (int j=0; j<4; j++) {
    switch (j) {
      case 0: pass->addNote( inputRanks[switchRank], octave ); break;
      case 1: pass->addNote( inputRanks[0], octave ); break;
      default: pass->addNote( inputRanks[j], octave ); break;
    }
  }
}


int modNeg(int number, int modulo) {
  return (number + modulo) % modulo;
}

/**
 * Calcule et ajoute à une passe les notes de la variation 4 ascendante
 */
void addPassNoteVar4DESC(Pass * pass, Melody * melody, int rankMax) {
  int inputRanks[3];
  int octaves[3];

  // Premiere note, calculé à partir du rang, du nombre de note et du nombre maximum de rang 
  int startInput = melody->inputLength * melody->octaveLength - 1;
  inputRanks[0] = (
    startInput - (pass->rank * 2)
  ) % melody->inputLength;
  octaves[0] =  (startInput - (pass->rank * 2)) / melody->inputLength + 1;

  // Deuxième note, enlève 2, gestion de l'octave enlevé
  if (inputRanks[0] - 2 < 0 ) {
    inputRanks[1] = abs(modNeg(inputRanks[0] - 2, melody->inputLength));
    octaves[1] = octaves[0] - 1;
  } else {
    inputRanks[1] = inputRanks[0] - 2;
    octaves[1] = octaves[0];
  }

  // Troisième note, ajoute 1, gestion de l'octave ajouté
  inputRanks[2] = (inputRanks[1] + 1) % melody->inputLength;
  octaves[2] = inputRanks[1] + 1 >= melody->inputLength
    ? octaves[1] + 1
    : octaves[1];

  // Cas particulier si seulement 1 input (les octaves peuvent passer de 2 en 2)
  if(melody->inputLength == 1) {
    inputRanks[0] = 0;
    inputRanks[1] = 0;
    inputRanks[2] = 0;
    octaves[0] = melody->octaveLength;
    octaves[1] = octaves[0] - 2 ;
    if(octaves[1] <= 0) {
      octaves[1] = melody->octaveLength - octaves[1];
    }
    octaves[2] = (octaves[1] + 1) % (melody->octaveLength);
  }

  // Cas particulier si seulement 1 octave et 2 notes (override)
  if(melody->octaveLength == 1) {
    octaves[0] = 1;
    octaves[1] = 1;
    octaves[2] = 1;
  }

  for (int i = 0; i<3; i++) {
    pass->addNote(melody->getInputAt(inputRanks[i])->degree, octaves[i]);
  }
}

/**
 * Rempli les paramètre d'une passe en se basant sur son précédent niveau et l'état de la mélodie
 */

void hydratePassDESC(Pass * pass, Pass * previousPass, Melody * melody) {
  int rankMax = melody->octaveLength;
  
  if ((melody->variation == 2 || melody->variation == 3) && melody->inputLength > 4) {
    rankMax *= 2;
  }

  if (melody->variation == 4) {
    rankMax = getVar4ASCRankMax(melody->inputLength, melody->octaveLength);
  }

  if (
    previousPass->direction == pass->direction
    && previousPass->variation == pass->variation
  ) {
    pass->rank = (previousPass->rank + 1) % rankMax;
  } else {
    pass->rank = 0;
  }

  switch (pass->variation) {
    case 1: addPassNoteVar1DESC(pass, melody); break;
    case 2: addPassNoteVar23DESC(pass, melody); break;
    case 3: addPassNoteVar23DESC(pass, melody); break;
    case 4: addPassNoteVar4DESC(pass, melody, rankMax); break;
    default: break;
  }
}
