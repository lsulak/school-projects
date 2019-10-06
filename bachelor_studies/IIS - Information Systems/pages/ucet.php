<div>
    <? if ($_SESSION["status"] != 1) { ?>
        Sekce je přístupná pouze přihlášeným uživatelům.
    <? } else { ?>
        <h2>Správa účtu</h2><br><br><br>
        <h3>Osobní údaje</h3>
        <?    $logged = $_SESSION["logged"];
        if ($_SESSION["role"] == 1) {
            $query = mysqli_query($db, "SELECT * FROM ctenar WHERE id='$logged'");
        } elseif ($_SESSION["role"] > 1) {
            $query = mysqli_query($db, "SELECT * FROM zamestnanec WHERE id='$logged'");
        }
        while ($row = mysqli_fetch_array($query)) {
            echo "<div id='ucet'>
			<img src='images/users.png' id='users_img' alt='User Info' width='60' height='60' />
			<b>Login: </b><p id='c1'>" . $row["login"] . "</p><br>";
            echo "<b>E-mail: </b><p id='c2'>" . $row["email"] . "</p><br>";
            echo "<b>Čas registrace: </b><p id='c3'>" . $row["cas_vytvoreni"] . "</p><br>";
            echo "<br>";
            echo "<b>Jméno: </b><p id='c4'>" . $row["jmeno"] . " " . $row["prijmeni"] . "</p><br>";
            echo "<b>Adresa: </b><p id='c5'>" . $row["adresa"] . "</p><br>";
            echo "<b>Telefon: </b><p id='c6'>" . $row["telefon"] . "</p><br></div>";
        }
        ?>
        <div id="ucet_moznosti">
            <h3>Možnosti účtu</h3>
            <img src="images/pass2.png" id="pass_img2" alt="Change Password" width="60" height="60"/>

            <p><a href="index.php?pg=set&what=1">Změna hesla</a> |
                <a href="index.php?pg=set&what=2">Změna osobních údajů</a></p><br>
        </div>
        <? if ($_SESSION['role'] == 1) { ?>
            <div id="ucet_moznosti2">
                <h3>Přehled</h3>
                <img src="images/book.ico" id="pass_img2" alt="Change Password" width="60" height="60"/>

                <p><a href="index.php?pg=show&what=1">Zobrazení mých aktualních výpůjček</a><br>
                    <a href="index.php?pg=manage&what=1">Správa rezervací</a></p><br>
            </div> <? } ?>




    <? } ?>
</div>