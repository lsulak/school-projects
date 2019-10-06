<div>
    <? if (!isset($_GET["id"])) {
        $query_str = "SELECT * FROM titul";
        if (isset($_GET["attr"]) && isset($_GET["desc"])) {
            $query_str .= " ORDER BY ";
            switch ($_GET["attr"]) {
                case 0:
                    $query_str .= "nazev";
                    break;
                case 1:
                    $query_str .= "rok_vydani";
                    break;
                case 2:
                    $query_str .= "cas";
                    break;
                default:
                    $query_str .= "nazev";
                    break;
            }
            if ($_GET["desc"] == 1) {
                $query_str .= " DESC";
            }
        } else {
            $query_str .= " ORDER BY nazev";
        }
        $tituly = mysqli_query($db, $query_str);
        ?>

        <div class="caption">Tituly v naší knihovně</div>
        <div id="table">
            <div class="header-row row">
                <span class="cell primary">Název</span>
                <span class="cell">Autor</span>
                <span class="cell">Rok vydání</span>
                <span class="cell">ISBN</span>
                <span class="cell">Nakladatelsví</span>
                <span class="cell">Dostupnost</span>
            </div>


            <?

            while ($row = mysqli_fetch_array($tituly)) {
                $t_id = $row["id"];
                $autor_titul = mysqli_query($db, "SELECT id_autor FROM autor_titul WHERE id_titul='$t_id'");
                ?>
                <div class="row">
					<span class="cell primary" data-label="nazev"><? echo
                            '<a href="index.php?pg=titul&id=' . $row["id"] . '">' . $row["nazev"] . '</a>'; ?></span>
                    <tr><span class="cell" data-label="autor">
						<? while ($row_at = mysqli_fetch_array($autor_titul)) {
                            $a_id = $row_at["id_autor"];
                            $autor = mysqli_query($db, "SELECT * FROM autor WHERE id='$a_id'");
                            $row_a = mysqli_fetch_assoc($autor); ?>
                        <a title="Zobrazit knihy autora" href='index.php?pg=tituly&id=<? echo $row_a["id"]; ?>'>
                            <? echo $row_a["prijmeni"] . ", " . $row_a["jmeno"] ?></a><?
                        }?>
						</span>
                        <span class='cell' data-label='rok_vydani'> <? echo $row["rok_vydani"] ?> </span>
                        <span class='cell' data-label='isbn'> <? echo $row["isbn"] ?></span>
                        <span class='cell' data-label='nakladatelstvi'> <? echo $row["nakladatelstvi"] ?></span>
                    </tr>
					<span class='cell' data-label='dostupnost'>
					<? include 'pages/inc_rez.php'; ?>
					</span>

                </div>

                <? ;
            } ?>
        </div>
        <div class="zoradenie">
            <form action="_sort.php" method="post">
                <select name="attr">
                    <option value="0">Název</option>
                    <option <? if ($_GET["attr"] == 1) echo 'selected="selected" '; ?>value="1">Rok vydání</option>
                    <option <? if ($_GET["attr"] == 2) echo 'selected="selected" '; ?>value="2">Čas přidání</option>
                </select>
                <select name="desc">
                    <option value="0">Vzestupně</option>
                    <option <? if ($_GET["desc"] == 1) echo 'selected="selected" '; ?>value="1">Sestupně</option>
                </select>
                <input id="zobraz_tituly" type='submit' name='submit' value='Zobrazit'/>

                <p>Seřadit podle: </p>
            </form>
        </div>

        <? ;
    } else {
        $a_id = $_GET["id"];
        $autor = mysqli_query($db, "SELECT * FROM autor WHERE id='$a_id'");
        while ($row = mysqli_fetch_array($autor)) {
            $autor_titul = mysqli_query($db, "SELECT id_titul FROM autor_titul WHERE id_autor='$a_id'");?>
            <h1><? echo $row["jmeno"] . " " . $row["prijmeni"] ?></h1>
            <p><a href="index.php?pg=tituly">(Zobrazit všechny naše tituly)</a></p><?
            $nar = $row["datum_narozeni"];
            if (strlen($nar) == 7)
                $nar = "0" . $nar;
            $zem = $row["datum_umrti"];
            if (strlen($zem) == 7)
                $zem = "0" . $zem;
            echo "<h3>Narození:</h3><p> " . substr($nar, 0, 2) . ". " . substr($nar, 2, 2) . ". " . substr($nar, 4) . "</p>";
            if ($zem != 0)
                echo "<h3>Úmrtí:</h3><p> " . substr($zem, 0, 2) . ". " . substr($zem, 2, 2) . ". " . substr($zem, 4) . "</p>";
            ?>
            <h3><p>Vice info
                    <a href="https://cs.wikipedia.org/wiki/<? echo $row["jmeno"] . " " . $row["prijmeni"] ?>">
                        zde</a></p></h3>
            <? if (mysqli_num_rows($autor_titul) > 0) { ?>
                <div class="caption">Tituly autora <? echo $row["jmeno"] . " " . $row["prijmeni"] ?></div>
                <div id="table">
                    <div class="header-row row">
                        <span class="cell primary">Název</span>
                        <span class="cell">Rok vydání</span>
                        <span class="cell">ISBN</span>
                        <span class="cell">Nakladatelsví</span>
                        <span class="cell">Dostupnost</span>
                    </div>

                    <?
                    $to_sort = array();
                    $cnt = 0;
                    while ($row_at = mysqli_fetch_array($autor_titul)) {
                        $t_id = $row_at["id_titul"];
                        $tituly = mysqli_query($db, "SELECT * FROM titul WHERE id='$t_id'");
                        $to_sort[$cnt] = mysqli_fetch_assoc($tituly);
                        $cnt++;
                    }
                    function asc($key)
                    {
                        return function ($a, $b) use ($key) {
                            return strnatcmp($a[$key], $b[$key]);
                        };
                    }

                    function desc($key)
                    {
                        return function ($b, $a) use ($key) {
                            return strnatcmp($a[$key], $b[$key]);
                        };
                    }

                    if (isset($_GET['attr']) && isset($_GET['desc'])) {
                        switch ($_GET['attr']) {
                            case 0:
                                $key = 'nazev';
                                break;
                            case 1:
                                $key = 'rok_vydani';
                                break;
                            case 2:
                                $key = 'cas';
                                break;
                            default:
                                $key = 'nazev';
                                break;
                        }
                        if ($_GET['desc'] == 0)
                            usort($to_sort, asc($key));
                        else
                            usort($to_sort, desc($key));
                    }

                    foreach ($to_sort as $value) {


                        ?>
                        <div class="row">
                        <span class="cell primary" data-label="nazev"><a
                                href="index.php?pg=titul&id=<? echo $value['id']; ?>"><? echo $value["nazev"] ?></a></span>
                        <span class="cell" data-label="rok_vydani"> <? echo $value["rok_vydani"] ?></span>
                        <span class="cell" data-label="isbn"> <? echo $value["isbn"] ?></span>
                        <span class="cell" data-label="nakladatelstvi"> <? echo $value["nakladatelstvi"] ?></span>
                        <span class="cell" data-label="dostupnost"><? include 'pages/inc_rez.php'; ?></span></div>
                    <? } ?>
                </div>
                <div class="zoradenie">
                    <form action="_sort.php" method="post">
                        <input type="hidden" name="autor" value="<? echo $a_id; ?>"/>
                        <select name="attr">
                            <option value="0">Nazev</option>
                            <option <? if ($_GET["attr"] == 1) echo 'selected="selected" '; ?>value="1">Rok vydání
                            </option>
                            <option <? if ($_GET["attr"] == 2) echo 'selected="selected" '; ?>value="2">Čas přidání
                            </option>
                        </select>
                        <select name="desc">
                            <option value="0">Vzestupně</option>
                            <option <? if ($_GET["desc"] == 1) echo 'selected="selected" '; ?>value="1">Sestupně
                            </option>
                        </select>
                        <input id="zobraz_tituly" type='submit' name='submit' value='Zobrazit'/>

                        <p>Seřadit podle: </p>
                    </form>
                </div>
            <? } else { ?>
                <p>Tento autor nemá v naší databázi žadné tituly</p>
            <? } ?>
        <? } ?>
    <? } ?>
</div>