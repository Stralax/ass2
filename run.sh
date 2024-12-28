#!/bin/bash

# Preveri, ali je bil podan argument za ime vhodne datoteke
if [ -z "$1" ]; then
    echo "Prosimo, podajte ime vhodne datoteke kot argument."
    exit 1
fi

# Preveri, ali vhodna datoteka obstaja
if [ ! -f "$1" ]; then
    echo "Vhodna datoteka '$1' ne obstaja!"
    exit 1
fi

# Procesiraj vhodno datoteko:
# - odstrani vrstice, ki se začnejo z M,
# - odstrani zadnjo vrstico,
# - odstrani prvo vrstico,
# - ohrani stolpce od 10 naprej,
# - združi vse vrstice v en sam red.
grep -v '^M' "$1" | head -n -1 | tail -n +2 | cut -c 10- | tr -d '\n' > Koda.txt

# Dodaj nov zlom vrstice na konec
echo -e "\n" >> Koda.txt

# Sporoči, da je proces končan
echo "Rezultat je bil shranjen v Koda.txt"
echo
echo "STRAT"

gcc sic2.c
./a.out

