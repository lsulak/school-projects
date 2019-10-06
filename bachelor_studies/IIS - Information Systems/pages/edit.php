<div>
    <? if ($_SESSION['role'] >= 2) { ?>
        <?if ($_GET['co'] == "autor") {
            $a_id = $_POST['autor'];
            if (isset($_GET['id']))
                $a_id = $_GET['id'];
            $autor = array();?>
            <h2>Editace autora</h2>
            <form action="index.php?pg=edit&co=autor" id="reg" method="post">
                Vyberte autora: <br>
                <select name='autor' onchange='this.form.submit()'>
                            <option value="0">--------</option>
                    <?
                    $autori = mysqli_query($db, "SELECT * FROM autor ORDER BY prijmeni");
                    while ($row = mysqli_fetch_array($autori)) {
                        echo "<option value='" . $row['id'] . "' ";
                        if ($a_id == $row['id']) {
                            $autor = $row;
                            echo "selected='selected'";

                        }
                        echo ">" . $row['prijmeni'] . ", " . $row['jmeno'] . "</option>";
                    }

                    ?>
                </select>
            </form>
            <?
            if ((isset($_POST['autor']) && $_POST['autor']!="0") || isset($_GET['id'])) {
                if (strlen($autor["datum_umrti"]) == 1)
                    $autor["datum_umrti"] = "";
                if (strlen($autor["datum_umrti"]) == 7)
                    $autor["datum_umrti"] = "0" . $autor["datum_umrti"];
                if (strlen($autor['datum_narozeni']) == 7)
                    $autor['datum_narozeni'] = "0" . $autor['datum_narozeni'];
                ?>
                <br><br>
                <form action="_add_author.php" id="reg" method="post">
                    *Příjmení: <br>
                    <input maxlength="50" id="inputtext2" type="text" name="prijmeni"
                           value='<? echo $autor["prijmeni"]; ?>'/><br>
                    *Jméno: <br>
                    <input maxlength="50" id="inputtext2" type="text" name="jmeno" value='<? echo $autor["jmeno"]; ?>'/><br>
                    *Datum narození (DDMMYYYY): <br>
                    <input maxlength="8" id="inputtext2" type="text" name="nar"
                           value='<? echo $autor["datum_narozeni"]; ?>'/><br>
                    Datum úmrtí (DDMMYYYY): <br>
                    <input maxlength="8" id="inputtext2" type="text" name="umr"
                           value='<? echo $autor["datum_umrti"]; ?>'/><br>
                    * jsou označeny povinné položky<br><br>
                    <input type="hidden" name="id" value="<? echo $a_id ?>"/>
                    <input type="hidden" name="edit" value="1"/>
                    <input type="submit" value="Editovat"/><br>
                </form>
            <?
            }
        } else if ($_GET['co'] == "titul") {
            $t_id = $_POST['titul'];
            if (isset($_GET['id']))
                $t_id = $_GET['id'];
            $titul = array();?>
            <h2>Editace titulu</h2>
            <form action="index.php?pg=edit&co=titul" id="reg" method="post">
                Vyberte titul: <br>
                <select name='titul' onchange='this.form.submit()'>
                    <option value="0">--------</option>
                    <?
                    $tituly = mysqli_query($db, "SELECT * FROM titul ORDER BY nazev");
                    while ($row = mysqli_fetch_array($tituly)) {
                        echo "<option value='" . $row['id'] . "' ";
                        if ($t_id == $row['id']) {
                            $titul = $row;
                            echo "selected='selected'";
                        }
                        echo ">" . $row['nazev'] . "</option>";
                    }
                    ?>
                </select>
            </form>
            <?
            if ((isset($_POST['titul']) && $_POST['titul']!="0") || isset($_GET['id'])) {
                $a_t = mysqli_query($db, "SELECT id_autor FROM autor_titul WHERE id_titul='$t_id'");
                $autori = array();
                while ($row2 = mysqli_fetch_array($a_t)) {
                    array_push($autori, $row2['id_autor']);
                }
                $aut_jmena = array();
                foreach ($autori as $autor) {
                    $aut = mysqli_query($db, "SELECT jmeno, prijmeni FROM autor WHERE id='$autor'");
                    $row_a = mysqli_fetch_assoc($aut);
                    $push = $row_a['jmeno'] . " " . $row_a['prijmeni'];
                    array_push($aut_jmena, $push);
                }
                ?>
                <br><br>
                <h3>Editovat titul</h3>
                <form action="_add_titul.php" id="reg" method="post">
                    *Název: <br>
                    <input maxlength="50" id="inputtext2" type="text" name="nazev" value='<? echo $titul["nazev"]; ?>'/><br>
                    **Autor(i): <br>
                    <input maxlength="250" id="inputtext2" type="text" name="autor"
                           value='<? echo implode(",", $aut_jmena); ?>'/><br>
                    *Rok vydání: <br>
                    <input maxlength="4" id="inputtext2" type="text" name="vydani"
                           value='<? echo $titul["rok_vydani"]; ?>'/><br>
                    ISBN: <br>
                    <input maxlength="30" id="inputtext2" type="text" name="isbn"
                           value='<? echo $titul["isbn"]; ?>'/><br>
                    Nakladatelství: <br>
                    <input maxlength="50" id="inputtext2" type="text" name="nakladatelstvi"
                           value='<? echo $titul["nakladatelstvi"]; ?>'/><br>
                    * jsou označeny povinné položky<br>
                    **Více autorů zadávejte ve formátu 'Jméno Příjmení,Jméno Prostřední Příjmení,....'<br><br>
                    <input type="hidden" name="id" value="<? echo $t_id ?>"/>
                    <input type="hidden" name="edit" value="1"/>
                    <input type="submit" value="Editovat"/><br>
                </form>


            <? }
        } ?>
    <?
    } else {
        echo "Stranka dostupná pouze zaměstnancům.";
    }?>

</div>