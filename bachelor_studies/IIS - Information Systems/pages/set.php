<div>
    <? if ($_SESSION["status"] != 1) { ?>
        Sekce je přístupná pouze přihlášeným uživatelům.
    <? } else { ?>
        <h2>Změna údajů</h2>

        <? if ($_GET["what"] == 1) { ?>

            <form id="acc_change" action="_set_password.php" method="post">
                <h3>Změna hesla</h3>
                Staré heslo:<br>
                <input maxlength="32" name="stare" type="password" value=""/><br>
                Nové heslo:<br>
                <input maxlength="32" name="nove" type="password" value=""/><br>
                Nové heslo pro kontrolu:<br>
                <input maxlength="33" name="nove_check" type="password" value=""/><br>
                <input type="submit" value="Změnit"/>
            </form>
            <input id="back" type="submit" value="Zpět" onclick="parent.location='index.php?pg=ucet'"/>

        <?
        } elseif ($_GET["what"] == 2) {
            $lg = $_SESSION["logged"];
            if ($_SESSION["role"] == 1)
                $query = mysqli_query($db, "SELECT email, adresa, telefon FROM ctenar WHERE id='$lg'");
            else
                $query = mysqli_query($db, "SELECT email, adresa, telefon FROM zamestnanec WHERE id='$lg'");
            while ($row = mysqli_fetch_array($query)) {
                ?>

                <form id="acc_change" action="_set_contact.php" method="post">
                    <h3>Změna kontaktních údajů</h3>

                    <p>Změnu jména a příjmení uživatele lze provést osobně na naší pobočce.</p>
                    E-mail: <br>
                    <input maxlength="50" name="email" type="text" value="<? echo $row['email']; ?>"/><br>
                    Adresa: <br>
                    <input maxlength="250" name="adresa" type="text" value="<? echo $row['adresa']; ?>"/><br>
                    Telefon: <br>
                    <input maxlength="14" name="telefon" type="text" value="<? echo $row['telefon']; ?>"/><br>
                    <br>
                    Heslo pro potvrzení změn: <br>
                    <input maxlength="32" name="heslo" type="password" value=""/><br>
                    <input type="submit" value="Změnit"/>
                </form>
                <input id="back" type="submit" value="Zpět" onclick="parent.location='index.php?pg=ucet'"/>
            <?
            }//while
        } //elseif
    }//else
    ?>
</div>