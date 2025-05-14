# BOT-UDP
Bot UDP sending a user datagram protocol from client to serwer. Availability serwer windows, client availability in linux.
Krótki opis rozwiązania zadania rekrutacyjnego.  
1.	Projekt zawiera dwie aplikacje konsolowe. 
a.	Aplikacja klienta, która loguje przed użytkownikiem komunikaty o wysłanych wysłaniu danych, generuje dane, zapisuje w pliku log.txt wysłane parametry. Napisana została na Linux a do kompilacji podstawowy kompilator gcc, dodatkowo załączony został w rozwiązaniu plik make file do łatwiejszego kompilowania
b.	Aplikacja serwera – odbiera dane od klienta i loguje dane na ekranie przed użytkownikiem. Napisana została na Windows. 
W rozwiązaniu dokonałem wyboru aby projekt został po części napisany w Linux a po części w Windows ze względu na wieloplatformowość, większą trudność i różnorodność bibliotek wykorzystywanych podczas rozwiązania. 
2.	Parametry przekazywane do aplikacji względem treści zadania. 
a.	Adres IP serwera
b.	Port UDP serwera
c.	Cykl przesyłania danych (w sekundach) 
d.	Cykl logowania danych (w sekundach)
e.	Wskazanie ścieżki i pliku logu
W zadaniu nie zostało doprecyzowane czy dane wskazanie ścieżki i pliku logu, logowania danych ma znajdować się po stronie serwera czy po stronie użytkownika. System został zaprojektowany tak aby logowanie wysyłanych danych pokazywało się zarówno u klienta i na serwerze. Klient natomiast loguje dodatkowo domyślnie w pliku log.txt dane generowane w momencie cyklu logowania danych. 
3.	Struktura danych jest prostą strukturą danych stosu zawierającą pola:
a.	Znacznik czasowy pomiaru (timestamp; dokładność 1s)
b.	Temperatura (w zakresie 20 – 120 stopni Celsjusza; dokładność 0,1 stopnia; wartość losowana w momencie nadawania komunikatu do odbiorcy)
c.	Status zasilania termometru (możliwe stany: zasilanie sieciowe, zasilanie bateryjne; losowane w momencie nadawania komunikatu do odbiorcy) 
d.	Identyfikator pomiaru (wartość rosnąca przy każdym nadaniu komunikatu do odbiorcy; zakres <1; UINT8(MAX)>)
e.	Suma kontrolna przesyłanego pakietu danych (jak najprostsza realizacja, przykładowo: suma poszczególnych bajtów struktury, CRC8, …)
Aplikacja została przetestowana na dwóch urządzeniach podłączonych do tej samej sieci lokalnej. Aby system Windows poprawnie odbierał pakiety UDP należy uprzednio odblokować port, na którym chcemy odbierać dane pakiety. 
