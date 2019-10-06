<?php
if ($_SESSION['role'] < 2)
    echo "Stranka dostupná pouze zaměstnancům.";
else {
    if ($_GET['co'] == "autor") {
        ?>
        <div>
            <h3>Přidání autora do databáze</h3>

            <form id="reg" action="_add_author.php" method="post">
                *Příjmení: <br>
                <input maxlength="50" id="inputtext2" type="text" name="prijmeni"
                       value='<? echo $_GET["prijmeni"]; ?>'/><br>
                *Jméno: <br>
                <input maxlength="50" id="inputtext2" type="text" name="jmeno" value='<? echo $_GET["jmeno"]; ?>'/><br>
                *Datum narození (DDMMYYYY): <br>
                <input maxlength="8" id="inputtext2" type="text" name="nar" value='<? echo $_GET["nar"]; ?>'/><br>
                Datum úmrtí (DDMMYYYY): <br>
                <input maxlength="8" id="inputtext2" type="text" name="umr" value='<? echo $_GET["umr"]; ?>'/><br>
                * jsou označeny povinné položky<br><br>
                <input type="hidden" name="edit" value="0"/>
                <input type="submit" value="Vložit"/><br>
            </form>
        </div>
    <? } else if ($_GET['co'] == "vytisk") { ?>
        <div>
            <h3>Přidání výtisku do databáze</h3>

            <form id="reg" action="_add_ex.php" method="post">
                *Pocet výtisků k přidání (maximálně 20 najednou): <br>
                <input maxlength="2" id="inputtext2" type="text" name="pocet" value='<? echo $_GET["pocet"]; ?>'/><br>
                *Pro titul: <br>
                <select name="titul">
                    <?
                    $tituly = mysqli_query($db, "SELECT id, nazev FROM titul ORDER BY nazev");
                    while ($row = mysqli_fetch_array($tituly)) {
                        echo "<option value='" . $row['id'] . "'>" . $row["nazev"] . "</option>";
                    }
                    ?>
                </select> <br>
                * jsou označeny povinné položky<br><br>
                <input type="submit" value="Vložit"/><br>
            </form>
        </div>
    <?
    } else if ($_GET['co'] == "vypujcka") {
        $uz_id = $_POST['id'];
        if (isset($_GET['id']))
            $uz_id = $_GET['id'];
        $titul = array();?>
        <h2>Vložení výpujčky</h2>
        <form action="index.php?pg=add&co=vypujcka2" id="reg" method="post">
            Vložte ID čtenáře (opište z kartičky)<br>[př:<12-22>; login ctenar = 24]: <br>
            <input maxlength="11" type="text" name="id" value="<? echo $uz_id; ?>"/><br>
            Počet vypůjčených knih (max. 20): <br>
            <input maxlength="2" type="text" name="pocet" value="<? echo $_GET['pocet']; ?>"/><br>
            <input type="submit" value="Dále"/>
        </form>    <br><br>
    <?
    } else if ($_GET['co'] == "vypujcka2") {
        if (isset($_GET['id']))
            $uz_id = $_GET['id'];
        else
            $uz_id = $_POST['id'];
        $ctenar = mysqli_query($db, "SELECT * FROM ctenar WHERE id='$uz_id'");
        $row = mysqli_fetch_assoc($ctenar);
        if ($row == NULL)
            header("location:http://iis.pesvan.cz/index.php?pg=add&co=vypujcka&msg=40");
        if (isset($_GET['pocet']))
            $pocet = $_GET['pocet'];
        else
            $pocet = $_POST['pocet'];
        if (!is_numeric($pocet) || $pocet < 0 || $pocet > 20)
            header("location:http://iis.pesvan.cz/index.php?pg=add&co=vypujcka&msg=41");
        ?>
        <form action="_add_bo.php?id=<? echo $uz_id; ?>" id="reg" method="post">
            ID čtenáře: <? echo $uz_id; ?><br>
            Nyní oskenujte ID výtisků:<br>
            [příklady výtisků: <3-18>]<br>
            <?
            for ($a = 0; $a < $pocet; $a++) {
                echo "(" . $a . ")<br>";
                echo "<input maxlength='11' type='text' name='" . $a . "' value='" . $_GET[$a] . "'/><br>";
            }
            ?>
            <input type="submit" value="Zpracovat"/><br>
            [příklady výtisků: <3-18>]
        </form>
    <? } else if ($_GET['co'] == "zamestnanec") { ?>

        <div><h3>Přidání zaměstnance do databáze</h3>

            <form id="reg" action="_add_zamestnanec.php" method="post">
                *Login (5-32 znaků): <br>
                <input maxlength="32" id="inputtext2" type="text" name="login" value='<? echo $_GET["login"]; ?>'/><br>
                *Heslo (6-32 znaků): <br>
                <input maxlength="32" id="inputtext2" type="password" name="heslo" value=''/><br>
                *Potvrzení hesla: <br>
                <input maxlength="33" id="inputtext2" type="password" name="pass_check" value=''/><br>
                *Email: <br>
                <input maxlength="50" id="inputtext2" type="text" name="email" value='<? echo $_GET["email"]; ?>'/><br>
                *Jméno: <br>
                <input maxlength="50" id="inputtext2" type="text" name="jmeno" value='<? echo $_GET["jmeno"]; ?>'/><br>
                *Příjmení: <br>
                <input maxlength="50" id="inputtext2" type="text" name="prijmeni"
                       value='<? echo $_GET["prijmeni"]; ?>'/><br>
                Adresa: <br>
                <input maxlength="200" id="inputtext2" type="text" name="adresa"
                       value='<? echo $_GET["adresa"]; ?>'/><br>
                Telefon: <br>
                <input maxlength="14" id="inputtext2" type="text" name="telefon"
                       value='<? echo $_GET["telefon"]; ?>'/><br>
                * jsou označeny povinné položky<br><br>
                <input type="hidden" name="edit" value="0"/>
                <input type="submit" value="Pridat"/><br>
            </form>
        </div>

    <? } else if ($_GET['co'] == "titul") { ?>
        <div><h3>Přidat titul do databáze</h3>

            <form id="reg" action="_add_titul.php" method="post">
                *Název: <br>
                <input maxlength="100" id="inputtext2" type="text" name="nazev" value='<? echo $_GET["nazev"]; ?>'/><br>
                **Autor <a href="javascript:openWin()">[Zobrazit autory v databázi]</a> <br>
                <input maxlength="250" id="inputtext2" type="text" name="autor" value='<? echo $_GET["autor"]; ?>'/><br>
                *Rok vydání: <br>
                <input maxlength="4" id="inputtext2" type="text" name="vydani" value='<? echo $_GET["vydani"]; ?>'/><br>
                ISBN: <br>
                <input maxlength="30" id="inputtext2" type="text" name="isbn" value='<? echo $_GET["isbn"]; ?>'/><br>
                Nakladatelství: <br>
                <input maxlength="50" id="inputtext2" type="text" name="nakladatelstvi"
                       value='<? echo $_GET["nakladatelstvi"]; ?>'/><br>
                <input type='hidden' name='edit' value='0'/>
                <input type="hidden" name="id" value="<? echo $t_id ?>"/>
                * jsou označeny povinné položky<br>
                **Více autorů zadávejte ve formátu 'Jméno Příjmení,Jméno Prostřední Příjmení' bez apostrofů<br><br>
                <input type="submit" value="Vložit"/><br>
            </form>
        </div>
    <? } ?>
<? } ?>