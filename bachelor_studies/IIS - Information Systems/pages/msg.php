<?php
if (isset($_GET["msg"])) {
    $msg = "Zpráva pro tuto událost ještě nebyla definována. Kód zprávy: " . $_GET["msg"];
    switch ($_GET["msg"]) {
        case 101:
            $msg = "Registrace proběhla úspěšně. Na Vámi zadaný email došly informace o registraci. Nezapoměňte se u nás ukázat!";
            break;
        case 102:
            $msg = "Přihlášení uživatele problěhlo úspěšně.";
            break;
        case 103:
            $msg = "Odhlášen!";
            break;
        case 104:
            $msg = "Heslo pro přístup k našemu systému Vám bylo opětovně zasláno na Váš email!";
            break;
        case 105:
            $msg = "Heslo bylo změněno.";
            break;
        case 106:
            $msg = "Údaje byly změněny.";
            break;
        case 107:
            $msg = "Autor byl úspěsně vložen do databáze.";
            break;
        case 108:
            $msg = "Titul byl úspěsně vložen do databáze.";
            break;
        case 109:
            $msg = "Autor byl úspěsně smazán z databáze.";
            break;
        case 110:
            $msg = "Titul byl úspěsně smazán z databáze.";
            break;
        case 111:
            $msg = "Výtisk(y) byl(y) úspěsně přidán(y) do databáze. <a href='index.php?pg=add&co=vytisk'>Přidat další</a>";
            break;
        case 112:
            $msg = "Rezervace proběhla úspěšně.";
            break;
        case 113:
            $msg = "Rezervace potvrzena.";
            break;
        case 114:
            $msg = "Rezervace odstraněna.";
            break;
        case 115:
            $msg = "Rezervace zamítnuta.";
            break;
        case 116:
            $msg = "Autor úspěsně editovan.";
            break;
        case 117:
            $msg = "Titul úspěsně editovan.";
            break;
        case 118:
            $msg = "Zaznam o vypujceni byl vložen do databáze.";
            break;
        case 119;
            $msg = "Výpujčka byla úspěsně prodlouzena.";
            break;
        case 120;
            $msg = "Výpujčka vracena.";
            break;
        case 121;
            $msg = "Nový zaměstnanec byl vytvořen.";
            break;
        case 122:
            $msg = "Zaměstnanec byl úspěsně smazán z databáze.";
            break;
        case 123:
            $msg = "Výtisk byl vyrazen z databáze.";
            break;
        case 199:
            $msg = "Databáze byla restartována do původního referenčního nastavení";
            break;

        //Chybove spravy!
        case 5:
            $msg = "CHYBA: login musí mít minimálně 5 a maximálně 32 znaků.";
            break;
        case 6:
            $msg = "CHYBA: tento login již nachází v databázi, zvolte jiný.";
            break;
        case 7:
            $msg = "CHYBA: heslo musí mít minimálně 6 a maximálně 32 znaků.";
            break;
        case 8:
            $msg = "CHYBA: zadaná hesla se neshodují.";
            break;
        case 9:
            $msg = "CHYBA: nezadán email.";
            break;
        case 10:
            $msg = "CHYBA: tento email již nachází v databázi.";
            break;
        case 11:
            $msg = "CHYBA: nezadáno jméno.";
            break;
        case 12:
            $msg = "CHYBA: nezadáno příjmení.";
            break;
        case 13:
            $msg = "CHYBA: nesprávný login/heslo!";
            break;
        case 14:
            $msg = "CHYBA: nezadán email!";
            break;
        case 15:
            $msg = "CHYBA: neplatný email!";
            break;
        case 16:
            $msg = "CHYBA: zadaný titul nebyl nalezen v databázi!";
            break;
        case 17:
            $msg = "CHYBA: heslo musí mít minimálně 6 a maximálně 32 znaků.";
            break;
        case 18:
            $msg = "CHYBA: zadaná hesla se neshodují.";
            break;
        case 19:
            $msg = "CHYBA: špatně zadané staré heslo.";
            break;
        case 20:
            $msg = "CHYBA: nezadán email!";
            break;
        case 21:
            $msg = "CHYBA: tento email již nachází v databázi.";
            break;
        case 22:
            $msg = "CHYBA: špatně zadané heslo.";
            break;
        case 23:
            $msg = "CHYBA: nezadáno datum narozeni.";
            break;
        case 24:
            $msg = "CHYBA: špatný format data narozeni.";
            break;
        case 25:
            $msg = "CHYBA: špatný format data umrti.";
            break;
        case 26:
            $msg = "CHYBA: nezadán nebo spatne zadan pocet výtisku k vlozeni.";
            break;
        case 27:
            $msg = "CHYBA: nezadán název titulu.";
            break;
        case 28:
            $msg = "CHYBA: nezadán autor titulu.";
            break;
        case 29:
            $msg = "CHYBA: nezadán rok vydání titulu.";
            break;
        case 30:
            $msg = "CHYBA: špatně zadaný formát: rok vydání titulu.";
            break;
        case 31:
            $msg = "CHYBA: špatně zadaný formát ceny titulu.";
            break;
        case 32:
            $msg = "CHYBA: špatně zadaný formát jmena autora. Chybí jméno alebo príjmení.";
            break;
        case 33:
            $msg = "CHYBA: zadaný autor se nenachází v databázi, nebo byla použita špatná syntax autorů (viz dole). <br>Pokud autor není v databázi, je třeba jej nejprve vytvořit.";
            break;
        case 34:
            $msg = "CHYBA: zadaný autor má přidělené tituly. <br>Pro smazání autora zrušte jeho tituly.";
            break;
        case 35:
            $msg = "CHYBA: špatně zadaný počet výtisku k přidání do databáze";
            break;
        case 36:
            $msg = "CHYBA: rezervace selhala: titul není k dispozici nebo jej už máte rezervován.";
            break;
        case 37:
            $msg = "CHYBA: rezervace je dostupná pouze registrovaným uživatelům.";
            break;
        case 38:
            $msg = "CHYBA: nepovedlo se potvrdit rezervaci: rezervace nenalezena (špatný odkaz?).";
            break;
        case 39:
            $msg = "CHYBA: nepovedlo se odstranit rezervaci: rezervace nenalezena (špatný odkaz?).";
            break;
        case 40:
            $msg = "CHYBA: čtenář nenalezen v databázi.";
            break;
        case 41:
            $msg = "CHYBA: zadán neplatný počet titulů.";
            break;
        case 42:
            $msg = "CHYBA: ID výtisku na pozici " . $_GET['errpoz'] . " nebylo nalezeno v databázi.";
            break;
        case 43:
            $msg = "CHYBA: ID výtisku na pozici " . $_GET['errpoz'] . " není momentálně k dispozici.";
            break;
        case 44:
            $msg = "CHYBA: výpujčka již byla jednou prodloužena.";
            break;
        case 45:
            $msg = "CHYBA: zadaný zaměstnanec je administrator.<br> Pro jeho odstranění je nutné poradit se s vedením a odstranit ho přímo z databáze.";
            break;
        case 46:
            $msg = "CHYBA: je potřeba před smazáním titulu smazat výtisky.";
            break;
        case 47:
            $msg = "CHYBA: výtisk je momentalne vypůjčen, nelze jej smazat.";
            break;
        case 98:
            $msg = "CHYBA: nepovedlo se uvést databázi do původního stavu.";
            break;
        case 99:
            $msg = "CHYBA: problém při změně dat v databázi. Pokud to nepůjde později, kontaktujte administrátora.";
            break;
        default:
            break;

    }
}
if ($_GET["msg"] >= 100) {
    ?>
    <div id="msg">
        <? echo $msg . "<br>"; ?>
    </div>
    <? ;
} else { ?>

    <div id="msg2">
        <? echo $msg . "<br>"; ?>
    </div>

    <? ;
} ?>