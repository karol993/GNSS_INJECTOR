# Poprawka PPS — 2026-09-16

## Ustalenia

`INJECTOR_Task()` wywołuje `PPS_GenStart()` co sekundę. Poprzednia implementacja
za każdym razem wykonywała Stop PWM, CNT=0 i Start PWM, chociaż TIM3 już
samodzielnie generował przebieg 1 Hz / 100 ms HIGH. W zależności od chwili
wykonania pętli przesuwało to zbocze albo restartowało aktywny impuls po
sprzętowym przepełnieniu timera. Konwerter GPS_toDCF_G0 zwiększa czas na każdym
zaakceptowanym zboczu narastającym PPS; nie filtruje zbyt krótkiego odstępu
między tymi zboczami.

W logu odbiornika DCF są impulsy 165–179 us, brak przyrostu OK oraz błędy
TIMING_ERROR, TOO_MANY_BITS i START_BIT_ERROR. Są zgodne z hipotezą dodatkowego
PPS, ale bez pomiaru PB4 lub diagnostyki PPS konwertera nie dowodzą jej
wystąpienia w tym konkretnym przebiegu.

Przesłany log prawdziwego LC29H przechodzi z RMC V do RMC A o czasie PC
10:10:32 (UTC w RMC 08:10:31). RMC po FIX ma format HHMMSS.000 oraz końcówkę
,,,A,V jak obecny profil emulatora. Parser konwertera pobiera czas, status
i datę RMC; końcowe V nie jest statusem FIX. Różnice w GSV/GSA nie są
wykorzystywane przez ten parser. Profil emulatora pozostaje syntetyczny,
nie odtwarza pełnego rozruchu ani wszystkich danych LC29H. Timestamps PC
nie pozwalają wyznaczyć relacji fizycznego PPS do NMEA.

## Zmiana i weryfikacja

PPS_GenStart uruchamia timer tylko raz. Kolejne wywołania nie zmieniają
licznika, kanału ani GPIO. PPS_GenStop nadal zatrzymuje timer i wymusza LOW.
Niepowodzenie HAL start pozwala na ponowienie.

- Test regresji starego kodu: FAIL, przedwczesne zbocze/reset fazy.
- Test regresji poprawionego kodu: PASS (model HAL, nie pomiar sprzętowy).
- Istniejące testy time, scheduler, position, nmea, lc29h: PASS.
- Wszystkie Core/Src kompilują się ARM GCC 11.3.1 z -Wall -Wextra -Werror.
- Link Debug/GNSS_INJECTOR.elf: PASS; text=38884, data=424, bss=5568.
- Make pod Windows zakończył się błędem uruchomienia procesu 0xC0000142.
  Kompilacja Core/Src i link wykonane bezpośrednio narzędziami CubeIDE;
  obiekty HAL/startup pochodzą z istniejącego katalogu Debug.
- Flashowania i pomiaru sprzętowego nie wykonano.

## Próba na sprzęcie

Po wgraniu aktualnego ELF do emulatora, przez jego port sterujący:

```text
STOP
MODE LC29H
FIX A
PPS ON
OFFSET 200
START
STATUS
STATS
```

Do badania poprawności ramek pozostaw emisję na co najmniej 3 pełne minuty.
Domyślny czas jest symulowany; TIME można ustawić przed START.
Prześlij STATUS/STATS emulatora i nowy log odbiornika. Jeśli problem pozostaje,
potrzebny jest log PPSDBG/zdarzeń diagnostycznych GPS_toDCF_G0 albo jednoczesny
pomiar PPS PB4 i wyjścia DCF. Oczekiwany PPS: jedno zbocze narastające na
sekundę bez dodatkowych krótkich przerw w HIGH.
