Skupina: grp009

Clenovia: Marek Bertovic, Ladislav Sulak
Loginy:   xberto00, xsulak04

Popis: Konzolova aplikacia Bludisko. Po spusteni aplikacie sa vypise obsah priecinka ./examples, 
	   kde je ulozena mapa bludiska (moze ich byt viac) v textovej podobe. Aplikacia caka na vstup 
	   od uzivatela, pri zadani prikazu 'game meno_suboru' sa nacita subor do dvojrozmerneho pola
	   objektov, pricom na prvom riadku su rozmery bludiska, v rozmedzi od 20x20 do 50x50 policok.
	   Ak je nacitanie uspesne, hra zacina, aplikacia nadalej caka na vsatup od uzivatela.
	   Pripustne formaty hernych prikazov:
	   		left - pre otocenie hraca dolava
	   		right - pre otocenie hraca doprava
	   		step - pre pohyb hraca v smere dopredu
	   		take - hrac vezme kluc, ktory musi byt pred nim
	   		open - hrac sa pokusi otvorit branu - otvori ju iba ak ma aspon 1 kluc, a brana je pred nim
	   		keys - vypise na standardny vystup pocet klucov hraca
	   		show - vytiskne bludisko: s hracom, klucami, finalnym polickom, branami, a stenami
	   		close - ukonci aplikaciu

	   	Ak pocas hry uzivatel zada prikaz 'game meno_suboru', uzivatel bude upozorneny, o behu aktualnej hry. Pre zvolenie inej mapy bludiska musi hru ukoncit prikazom 'close'.