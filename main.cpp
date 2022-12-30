#include <iostream>

using std::cout;
using std::endl;

class iteratore; // basta dichiarare con una dichiarazione incompleta il nome della classe e risulta tutto apposto.

class contenitore { // peso: 8B * 100 = 800B.
  friend class iteratore;
private:
  int *array; 
  // bisognerebbe implementare degli helper method per poter fare la copia di un array dall'uno all'altro. 
  static int* copia(int*); 
public:
  static int size;
  contenitore();
  // dichiarazione (con seguente ridefinzione) a causa della regola del tre.
  ~contenitore(); // distruttore
  contenitore(const contenitore&); // costruttore di copia profondo
  contenitore& operator=(const contenitore&);  
  // dichiarazine operatori implementati dalla classe contenitore. 
  int& operator[](const iteratore&) const;
  iteratore begin() const;
  iteratore end() const;
};

class iteratore { // peso: 8B (dato che siamo su architettura a 64 bit).
  friend class contenitore;
private:
  int *puntatore; 
public:
  bool operator==(iteratore) const; 
  bool operator!=(iteratore) const;
  iteratore& operator++(); // operatore di incremento prefisso
  iteratore operator++(int); // operatore di incremento postfisso
  int* getAddress(); // questo non si deve fare nella pratica in quanto andrebbe a violare il principio dell'IH
  // dato che diamo informazioni concernenti la rappresentazione interna sia del contenitore sia dell'iteratore.
};

// IMPLEMENTAZIONE METODI CONTENITORE.
// definizione del costruttore
contenitore::contenitore() : array(new int[100]){ 
  for (int i = 0; i < 100; i++) {
    array[i] = i; // almento cosi' posso effettuare un controllo su quegli che sono i valori presenti all'interno della memoria.
  }

}

// ridefinizione del distruttore;
contenitore::~contenitore() {
  cout << "distruttore di contenitore" << endl;
  delete[] array; // distruzione profonda del campo dati array
  // questo in quanto vogliamo che l'array venga deallocato completamente.
}

contenitore::contenitore(const contenitore& other) : array(copia(other.array)) { } // costruttore di copia profondo

contenitore& contenitore::operator=(const contenitore& other) { // assegnazione profonda.
  if (this != &other) { // & di un riferimento prende l'indirizzo dell'area di memoria riferita (il che ha anche senso
    // data la definizione di riferimento).
    delete [] array;  
    array = copia(other.array);
  }
  return (*this);
}

int& contenitore::operator[](const iteratore& it) const {
  return *(it.puntatore);
}

iteratore contenitore::begin() const {
  iteratore aux;
  aux.puntatore = contenitore::array; 
  return aux;
}

iteratore contenitore::end() const {
  iteratore aux;
  aux.puntatore = (array + size); 
  return aux;
}

// definizione campi statici contenitore.
int contenitore::size = 100;

int* contenitore::copia(int* a) {
  int *array_di_ritorno = new int[size];
  for (int i = 0; i < size; i+=1) {
    array_di_ritorno[i] = a[i];
  }
  return array_di_ritorno;
}
// IMPLEMENTAZIONE METODI ITERATORE.
bool iteratore::operator==(iteratore it) const {
  return puntatore == it.puntatore;
}

bool iteratore::operator!=(iteratore it) const {
  return puntatore != it.puntatore;
}

iteratore& iteratore::operator++() { 
  if (puntatore) {
    puntatore+=1;
  }
  return (*this);
}

iteratore iteratore::operator++(int) {
  iteratore app = (*this);
  if (puntatore) {
    puntatore = puntatore + 1;
  }
  return app; // questo purtroppo rimane un' incognita.
}

void stampa_contenitore(contenitore x) { // x e' di classe automatica => deallocata (chiamata al distruttore di x) alla fine del blocco.

  for (iteratore it = x.begin(); it != x.end(); ++it) { // iteratore it viene costruito di copia dal temporaneo anonimo che viene ritornato dalla funzione begin.
    cout << x[it] << " "; 
  } 
  cout << "fine funzione ";
}

int main() {
  contenitore mio_contenitore;
  stampa_contenitore(mio_contenitore); // il problemino qui e' stronzo ma alla fine risulta evidente sia
  // NEL CASO IN CUI NON AVESSI RIDEFINITO IL COSTRUTTORE DI COPIA.
  // quando viene chiamata la funzione succede..
  // contenitore x = mio_contenitore => costruzione di compia di x da mio_contenitore.
  // il problema e' che la costruzione di copia standard di x da mio_contenitore si limita ad:
  //  1. allocare lo spazio per x.array 
  //  2. inizializzarlo al campo dati di mio_contenitore.
  // => si crea di conseguenza una situazione di aliasing in quanto sia x che mio_contenitore puntatno alla stessa area di memoria.
  // => quando viene invocato automaticamente il distruttore ridefinito della classe contenitore viene fatto due volte il tentativo diamo
  // deallocazione della stessa area di memoria. il primo ha successo tuttavia il secondo no a causa del primo.
  // => questo e' un esempio paradigmatico nel quale la regola del 3 vale.
  cout << "fine main ";
}










