<?php
if ($_SESSION['role'] < 2) {
    echo "Stranka dostupná pouze zaměstnancům.";
} else {
    if ($_GET['co'] == 'autor') {
        $autor = mysqli_query($db, "SELECT * FROM autor");
        $a_cnt = mysqli_num_rows($autor);
        ?>
        <div class="caption">Smazání autora</div>
        <div id="table2">
            <div class="header-row row">
                <span class="cell primary">Jméno</span>
                <span class="cell">Počet knih</span>
                <span class="cell">Zrušení</span>
            </div>
            <?while ($row = mysqli_fetch_array($autor)) {
                ?>
                <div id="del" class="row">
					<span class="cell primary" data-label="jmeno"><?
                        echo "<a href='index.php?pg=tituly&id=" . $row['id'] . "'>" . $row["prijmeni"] . ", " . $row["jmeno"] . "</a>"; ?></span>
					<span class="cell" data-label="pocet_knih"><?
                        $a_id = $row['id'];
                        echo mysqli_num_rows(mysqli_query($db, "SELECT * FROM autor_titul WHERE id_autor='$a_id'"));
                        ?></span>
					<span class="cell" data-label="zrusit"><a title="Zmazat autora"
                                                              href='_delete_author.php?id=<? echo $a_id ?>'>
                            Zrušit
                        </a></span>
                </div>
            <? } ?>
        </div>
    <?
    } else if ($_GET['co'] == 'vytisk') {
        $vytisk = mysqli_query($db, "SELECT * FROM vytisk");
        ?>
        <div class="caption">Vyřazení výtisku</div>
        <div id="table2">
            <div class="header-row row">
                <span class="cell primary">ID výtisku</span>
                <span class="cell">Název titulu</span>
                <span class="cell">Vyřadit z databáze</span>
            </div>
            <?while ($row = mysqli_fetch_array($vytisk)) {
                $v_id = $row['id'];
                $t_id = $row['id_titul'];
                $titul = mysqli_query($db, "SELECT nazev FROM titul WHERE id='$t_id'");
                $row2 = mysqli_fetch_assoc($titul);

                ?>
                <div id="del" class="row">
                    <span class="cell primary" data-label="id"><? echo $v_id; ?></span>
                    <span class="cell" data-label="nazev"><? echo $row2['nazev']; ?></span>
                    <span class="cell" data-label="zrusit"><a href='_rm_ex.php?id=<? echo $v_id ?>'>Zrušit</a></span>
                </div>
            <? } ?>
        </div>
    <?
    } else if ($_GET['co'] == 'titul') {
        $titul = mysqli_query($db, "SELECT * FROM titul");

        ?>
        <div class="caption">Smazání titulu</div>
        <div id="table2">
            <div class="header-row row">
                <span class="cell primary">Název</span>
                <span class="cell">Autor</span>
                <span class="cell">Zrušení</span>
            </div>
            <?while ($row = mysqli_fetch_array($titul)) {
                ?>
                <div id="del2" class="row">
				<span class="cell primary" data-label="jmeno"><?
                    echo "<a href='index.php?pg=titul&id=" . $row['id'] . "'>" . $row["nazev"] . "</a>"; ?></span>
                    <span class="cell" data-label="autor"><?
                        $t_id = $row['id'];

                        $id = mysqli_query($db, "SELECT id_autor FROM autor_titul WHERE id_titul='$t_id'");
                        while ($row2 = mysqli_fetch_array($id)) {
                            $rr = $row2['id_autor'];

                            $id2 = mysqli_query($db, "SELECT jmeno, prijmeni FROM autor WHERE id='$rr'");
                            while ($row3 = mysqli_fetch_array($id2)) {
                                echo "<a href='index.php?pg=tituly&id=" . $row['id'] . "'>" . $row3['prijmeni'] . ", " . $row3["jmeno"] . "</a><br>";
                            }
                        }
                        ?></span>
				<span class="cell" data-label="zrusit"><a href='_delete_titul.php?id=<? echo $t_id ?>'>
                        Zrušit
                    </a></span>
                </div>
            <? } ?>
        </div>
    <?
    } else if ($_GET['co'] == "zamestnanec") {
        $zamestnanec = mysqli_query($db, "SELECT * FROM zamestnanec");
        $z_cnt = mysqli_num_rows($zamestnanec);

        ?>
        <div class="caption">Smazaní zaměstnance</div>
        <div id="table2">
            <div class="header-row row">
                <span class="cell primary">Jméno</span>
                <span class="cell">Rok nástupu</span>
                <span class="cell">Zrušení</span>
            </div>
            <?
            while ($row = mysqli_fetch_array($zamestnanec)) {
                ?>
                <div id="del" class="row">
				<span class="cell primary" data-label="jmeno"><?
                    echo "<a href='index.php?pg=zamestnanec&id=" . $row['id'] . "'>" . $row["prijmeni"] . ", " . $row["jmeno"] . "</a>"; ?></span>
                    <span class="cell" data-label="rok_nastupu"><?
                        $z_id = $row['id'];
                        $prava = $row['prava'];
                        $nastup = $row["cas_vytvoreni"];
                        echo substr($nastup, 0, 4)
                        ?></span>
				<span class="cell" data-label="zrusit">
					<a title="zrusit zamestnance"
                       href='_delete_zam.php?id=<? echo $z_id ?>&prava=<? echo $prava ?>'>
                        Zrušit
                    </a></span>
                </div>
            <? } ?>
        </div>
    <? } ?>
<? } ?>