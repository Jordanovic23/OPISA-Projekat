# MAVN prevodilac

Prevodilac za jezik **MAVN** (Mali Asemblerski Viši Nivo) u **MIPS 32** asemblerski kod, napisan u C++-u.

> Ispitni rad iz predmeta **Osnovi paralelnog programiranja i softverski alati**
> Fakultet tehničkih nauka, Univerzitet u Novom Sadu
> Autor: **Aleksa Antić, RA 87/2024** · Mentor: dr Miodrag Đukić · Novi Sad, 2026.

---

## Sadržaj

- [O projektu](#o-projektu)
- [Faze prevođenja](#faze-prevođenja)
- [Jezik MAVN](#jezik-mavn)
- [Struktura projekta](#struktura-projekta)
- [Prevođenje i pokretanje](#prevođenje-i-pokretanje)
- [Primer](#primer)
- [Testni primeri](#testni-primeri)
- [Ograničenja](#ograničenja)
- [Dokumentacija](#dokumentacija)

---

## O projektu

MAVN je pojednostavljeni asemblerski jezik koji radi sa **registarskim promenljivim** (`r1`, `r2`, …) umesto sa stvarnim registrima procesora. Zadatak prevodioca je da izvorni program analizira, proveri njegovu ispravnost i svakoj registarskoj promenljivoj dodeli jedan od stvarnih MIPS registara (`$t0`–`$t3`), a zatim generiše izvršivi MIPS asemblerski kod (npr. za simulator QtSpim / MARS).

Pored osnovnog skupa instrukcija, prevodilac je proširen sa tri nove instrukcije: **`mul`**, **`or`** i **`beq`**.

## Faze prevođenja

```
 .mavn ──► Leksička ──► Sintaksna ──► Analiza životnog ──► Graf smetnji i ──► Generisanje ──► output.s
           analiza      analiza       veka promenljivih     alokacija reg.      MIPS koda
```

| Faza | Klasa | Opis |
|---|---|---|
| Leksička analiza | `LexicalAnalysis`, `FiniteStateMachine`, `Token` | Konačni automat (52 stanja) deli izvorni kod na tokene. |
| Sintaksna analiza | `SyntaxAnalysis` | Rekurzivni spust po gramatici jezika; usput gradi internu reprezentaciju (promenljive, instrukcije, labele). |
| Interna reprezentacija | `Variable`, `Instruction`, `Label` (`IR.h`) | Čuva tip i poziciju promenljivih, operande instrukcija, konstante, labele, kao i USE/DEF, IN/OUT i SUCC/PRED skupove. |
| Analiza životnog veka | `LivenessAnalysis` | Formira USE/DEF i SUCC/PRED skupove, a zatim iterativno računa IN/OUT skupove. |
| Graf smetnji i alokacija | `InterferenceGraph` | Gradi matricu smetnji, računa stepene čvorova, uprošćava graf (stek) i dodeljuje registre bojenjem grafa. |
| Generisanje koda | `CodeGenerator` | Ispisuje sekcije `.data` i `.text` u fajl `output.s`. |

Svaka faza na standardni izlaz ispisuje međurezultate (tokene, USE/DEF, IN/OUT, matricu smetnji, stek uprošćavanja, dodelu registara), što olakšava proveru rada.

## Jezik MAVN

### Gramatika

```
Q → S ; L
S → _mem mid num
S → _reg rid
S → _func id
S → id : E
S → E
L → eof | Q
E → add  rid, rid, rid
E → addi rid, rid, num
E → sub  rid, rid, rid
E → la   rid, mid
E → lw   rid, num(rid)
E → li   rid, num
E → sw   rid, num(rid)
E → b    id
E → bltz rid, id
E → nop
E → mul  rid, rid, rid      ← dodato
E → or   rid, rid, rid      ← dodato
E → beq  rid, rid, id       ← dodato
```

### Deklaracije

| Konstrukcija | Značenje |
|---|---|
| `_mem m1 6;` | Memorijska promenljiva `m1` sa početnom vrednošću 6 (ide u `.data`). |
| `_reg r1;` | Registarska promenljiva `r1` (dobija stvarni registar pri alokaciji). |
| `_func main;` | Početak funkcije / ulazna tačka. |
| `labela:` | Labela za instrukcije skoka. |
| `// ...` | Jednolinijski komentar (do kraja reda). |

### Instrukcije

| Instrukcija | Opis |
|---|---|
| `add rd, rs, rt` | `rd = rs + rt` |
| `addi rd, rs, num` | `rd = rs + num` |
| `sub rd, rs, rt` | `rd = rs - rt` |
| `mul rd, rs, rt` | `rd = rs * rt` *(nova)* |
| `or rd, rs, rt` | `rd = rs \| rt` *(nova)* |
| `la rd, mid` | Učitava adresu memorijske promenljive |
| `li rd, num` | Učitava konstantu |
| `lw rd, num(rs)` | Čitanje iz memorije |
| `sw rs, num(rd)` | Upis u memoriju |
| `b labela` | Bezuslovni skok |
| `bltz rs, labela` | Skok ako je `rs < 0` |
| `beq rs, rt, labela` | Skok ako je `rs == rt` *(nova)* |
| `nop` | Prazna instrukcija |

## Struktura projekta

```
Projekat/
├── Dokumentacija_projekta.pdf / .doc   # Dokumentacija rada
├── doc/
│   ├── projekat.pdf                    # Postavka zadatka
│   ├── MIPS-instructions.pdf           # Referenca MIPS instrukcija
│   └── primer_dobre_dokumentacije.doc
├── examples/                           # Testni .mavn programi
└── src/
    ├── main.cpp                        # Pokreće sve faze redom
    ├── Constants.h / Types.h           # Konstante, tipovi tokena, instrukcija i registara
    ├── Token.*                         # Token
    ├── FiniteStateMachine.*            # Konačni automat za leksičku analizu
    ├── LexicalAnalysis.*               # Leksička analiza
    ├── SyntaxAnalysis.*                # Sintaksna analiza + izgradnja IR-a
    ├── IR.h                            # Variable, Instruction, Label
    ├── LivenessAnalysis.*              # Analiza životnog veka
    ├── InterferenceGraph.*             # Graf smetnji, uprošćavanje, alokacija registara
    ├── CodeGenerator.*                 # Generisanje MIPS koda
    ├── output.s                        # Primer generisanog izlaza
    └── LexicalAnalysis.sln / .vcxproj  # Visual Studio rešenje
```

## Prevođenje i pokretanje

### Visual Studio (preporučeno)

1. Otvoriti `Projekat/src/LexicalAnalysis.sln`.
2. Pokrenuti sa **Ctrl + F5** (Build & Run).
3. Generisani kod se upisuje u `output.s` u radnom direktorijumu projekta.

### Izbor ulaznog fajla

Ulazni fajl je zadat u `main.cpp`:

```cpp
std::string fileName = ".\\..\\examples\\simple.mavn";
```

Za drugi primer dovoljno je promeniti putanju (npr. na `multiply.mavn` ili `grananje.mavn`) i ponovo pokrenuti program.

### Ostali kompajleri (g++ / clang)

Projekat je pisan za MSVC. Za prevođenje drugim kompajlerom potrebno je:

- promeniti `void main()` u `int main()`,
- zameniti `\\` sa `/` u putanji do ulaznog fajla,

pa zatim:

```bash
cd Projekat/src
g++ -std=c++17 *.cpp -o mavn
./mavn
```

### Pokretanje generisanog koda

Fajl `output.s` može se učitati u **QtSpim** ili **MARS** simulator i izvršiti.

## Primer

**Ulaz** (`examples/simple.mavn`):

```asm
_mem m1 6;
_mem m2 5;

_reg r1;
_reg r2;
_reg r3;
_reg r4;
_reg r5;

_func main;
	la		r4, m1;
	lw		r1, 0(r4);
	la		r5, m2;
	lw		r2, 0(r5);
	add		r3, r1, r2;
```

**Izlaz** (`output.s`):

```asm
.globl main

.data
m1:		.word 6
m2:		.word 5

.text
main:
        la      $t0, m1
        lw      $t1, 0($t0)
        la      $t0, m2
        lw      $t0, 0($t0)
        add     $t0, $t1, $t0
```

Pet registarskih promenljivih smešteno je u samo dva stvarna registra, jer im se životni vekovi ne preklapaju.

## Testni primeri

| Fajl | Šta proverava |
|---|---|
| `simple.mavn` | Osnovno uspešno prevođenje |
| `multiply.mavn` | Petlja sa labelom i `bltz` (množenje uzastopnim sabiranjem) |
| `grananje.mavn` | Instrukcije grananja `beq` i `b` |
| `provera_analize_zivotnog.mavn` | Ispravnost IN/OUT skupova |
| `provera_alokacije.mavn` | Dodela registara |
| `nedovoljno registara.mavn` | Situacija kada graf ne može da se uprosti (potrebno prelivanje) |
| `leksicka_greska.mavn` | Prijava leksičke greške |
| `sintaksna_greska.mavn` | Prijava sintaksne greške |

## Ograničenja

- Na raspolaganju su **4 registra** (`$t0`–`$t3`), definisano sa `__REG_NUMBER__` u `Constants.h`.
- **Prelivanje (spill) nije implementirano** — ako graf smetnji ne može da se uprosti, prevodilac ispisuje grešku *„graf ne može da se uprosti, potrebno je prelivanje!”* i prekida rad.
- Ulazni i izlazni fajl su zadati u kodu, ne kao argumenti komandne linije.

## Dokumentacija

Detaljan opis rešenja (analiza problema, koncept, opis svake klase i faze, rezultati testiranja) nalazi se u [`Dokumentacija_projekta.pdf`](Projekat/Dokumentacija_projekta.pdf). Postavka zadatka je u [`doc/projekat.pdf`](Projekat/doc/projekat.pdf).
