#!/usr/bin/php
<?php
#XTD:xsulak04

//GLOBALNE PREMENNE - PRE PARAMETRE
$filename_in = NULL;
$filename_out = NULL;
$file_out = NULL;

$header = NULL;
$param_g = FALSE;
$param_a = FALSE;
$param_b = FALSE;
$n = -1;

//PREMENNA PRE KONKATENACIU S PODELEMENTOM, JEDNOZNACNE URCENIE PODELEMENTU
$index = 0;

//GLOBALNE PREMENNE - PRE VYTVORENIE POLI, FUNKCIE SA VOLAJU V CYKLE, ALE POLE POTRBUEJEME VYTVORIT IBA RAZ
$first_time_table = TRUE;
$first_time_table2 = TRUE;
$first_time_table3 = TRUE;
$first_time_table4 = TRUE;

//OSETRENIE NEJAKYCH SITUACII,VID POTREBNE FUNKCIE NEZSIE
$fix_bug_attr = TRUE;
$first_time_N = TRUE;

//FUNKCIA PRE VYPIS NAPOVEDY, SPUSTA SA PARAMETROM --help
function help()
{
    fputs(STDIN,
        "
        1. Projekt do predmetu IPP : varianta XDT
        PHP skript pre konverziu XML -> SQL
        Autor: Ladislav Sulak
        Login: xsulak04
        Datum: 2/2014
        Parametre:
        --help              pre napovedu
        --input=filename    ak ziadny, vstup z stdin
        --output=filename   ak ziadny, vystup na stdout
        --header='hlavicka' vlozi sa na zac. vystupneho suboru
        --etc=n             n>=0, max. pocet stlpcov vzniknutych
                            z podelementov rovnakeho nazvu
        -a                  nebudu sa generovat stlpce z atributu z XML
        -b                  nesmie byt skombinovane s --etc=n
        -g                  pozmeneny vystupny subor, vystup bude XML           ");
    exit(0);
}

//FUNKCIA PRE CHYBOVE VYSTUPY, VRATI PRISLUSNY KOD CHYBY
function err_msg($id_err)
{
    if($id_err == 1)
        fputs(STDERR, "Zly format parametrov, alebo ich zla kombinacia!\n");

    elseif($id_err == 2)
        fputs(STDERR, "Chyba pri pokuse o otvorenie(zatvorenie) vstupneho suboru! (neexistuje?)\n");

    elseif($id_err == 3)
        fputs(STDERR, "Chyba pri pokuse o otvorenie(zatvorenie) vystupneho suboru pre zapis!\n");

    elseif($id_err == 4)
        fputs(STDERR, "Chybny format vstupneho suboru!\n");

    elseif($id_err == 90)
        fputs(STDERR, "MENO ATRIBUTU SA ZHODUJE S MENOM PODELEMENTU!\n");

    exit($id_err);
}

//FUNKCIA PRE SPRACOVANIE PARAMETROV, NEMOZU SA OPAKOVAT
function params($arg_pole, $arg_pocet)
{
    $etc_present = FALSE;
    $b_present = FALSE;
    $a_present = FALSE;

    for($i = 1; $i < $arg_pocet; $i++)
    {
        if(strncmp($arg_pole[$i], "-", 1) != 0)
            return 1;

        elseif(strcmp($arg_pole[$i], "--help") == 0)
        {
            if($arg_pocet == 2)
            {
                help();
            }
            else
                return 1;
        }
        elseif(strncmp($arg_pole[$i], "--input=", 8) == 0)
        {
            $name_miss_in = substr($arg_pole[$i], -8);
            if($name_miss_in == "--input=")
                return 1;

            if($GLOBALS['filename_in'] == NULL)
                $GLOBALS['filename_in'] = substr($arg_pole[$i], 8);
            else
                return 1;
        }
        elseif(strncmp($arg_pole[$i], "--output=", 9) == 0)
        {
            $name_miss_out = substr($arg_pole[$i], -9);
            if($name_miss_out == "--output=")
                return 1;

            if($GLOBALS['filename_out'] == NULL)
                $GLOBALS['filename_out'] = substr($arg_pole[$i], 9);
            else
                return 1;
        }
        elseif(strncmp($arg_pole[$i], "--header=", 9) == 0)
        {
            if($GLOBALS['header'] == NULL)
                $GLOBALS['header'] = "--" . substr($arg_pole[$i], 9) . "\n\n";
            else
                return 1;
        }
        elseif(strncmp($arg_pole[$i], "--etc=", 6) == 0)
        {

            if($etc_present)
                return 1;

            $etc_present = TRUE;

            if($b_present) // parameter "-b" a "--etc=" nemozu byt zadane sucasne
                return 1;

            $name_miss_n = substr($arg_pole[$i], -6);
            if($name_miss_n == "--etc=")
                return 1;

            foreach($arg_pole as $conflict)
            {
                if(strcmp($conflict, "-b") == 0)
                    return 1;
            }
            $sprac_n = substr($arg_pole[$i], 6);

            for($j=0; $j < strlen($sprac_n); $j++)
            {
                if(!(ord($sprac_n[$j]) >= 48 && ord($sprac_n[$j]) <= 57))
                {
                    return 1;
                }
            }
            $GLOBALS['n'] = $sprac_n;
        }
        elseif(strncmp($arg_pole[$i], "-a", 2) == 0)
        {
            if($a_present)
                return 1;

            $a_present = TRUE;
            $GLOBALS['param_a'] = TRUE;
        }
        elseif(strncmp($arg_pole[$i], "-b", 2) == 0)
        {
            if($b_present)
                return 1;

            $b_present = TRUE;
            if($etc_present)
                return 1;  // parameter "-b" a "--etc=" nemozu byt zadane sucasne
            else
                $GLOBALS['param_b'] = TRUE;
        }
        elseif(strncmp($arg_pole[$i], "-g", 2) == 0)
        {
            if($GLOBALS['param_g'] == TRUE)
                return 1;

            $GLOBALS['param_g'] = TRUE;
        }
        else
            return 1;
    }
    return 0;
}

//FUNKCIA PRE VYTVORENIE HLAVNEHO POLA, VYUZIVA K TOMU DALSIE POLE, SPAJAJU SA REKURZIVNE
function elements_array($elem)
{
    if($GLOBALS['first_time_table'] == TRUE)
    {
        global $element_array;
        $element_array = array();
    }

    $GLOBALS['first_time_table'] = FALSE;
    if(!empty($GLOBALS['attr_array']))
    {
        $prvok = array($elem => (array)$GLOBALS['attr_array']);
        $GLOBALS['element_array'] = array_merge_recursive((array)$GLOBALS['element_array'], $prvok); //array_merge_recursively
    }
    unset($GLOBALS['attr_array']);
    $GLOBALS['attr_array'] = array();
}

//pomocne pole, vytvori a pouziva sa ked sa zada prepinac -g
function arr_relation()
{
    global $relation_array;
    $relation_array = array();
    foreach($GLOBALS['children_array'] as $element => $something)
        $relation_array[$element] = array();
}

//vyssie spominane pole pri relaciach sa vytvara z hlavneho pola, no ostanu tam iba podelementy
function arr_childrens()
{
    global $children_array;
    if($GLOBALS['n'] ==  -1)
        $children_array = $GLOBALS['element_array'];
    else
        $children_array = $GLOBALS['N_pole'];
    foreach($children_array as $kluc => $val)
        foreach($val as $cislo => $value)
        {
            if(($value == "BIT" || $value == "INT"
                    || $value == "FLOAT" || $value == "NVARCHAR"
                    || $value == "NTEXT") && !is_array($value))
                unset($children_array[$kluc][$cislo]);

            elseif(is_array($value))
                foreach($value as $k1 => $v1)
                {
                    $children_array[$kluc][$cislo][$k1] = strtolower($v1);

                    if($k1 == "parent")
                    {
                        $pole = array($k1 => $v1);
                        array_push($children_array[$kluc], $pole);
                    }
                    else
                    {
                        if(is_int($v1))
                            $children_array[$kluc][$cislo][$k1] = removeID($k1);
                        array_push($children_array[$kluc], $children_array[$kluc][$cislo][$k1]);
                    }
                    if(is_array($children_array[$kluc][$cislo]))
                        unset($children_array[$kluc][$cislo]);
                }
        }
}

//VYTVORENIE POMOCNEHO POLA PRE POMOCNE POLE PRE ATRIBUTY, VSETKY SA SPOJA DO HLAVNEHO POLA
function atrHelp_array()
{
    if($GLOBALS['first_time_table4'] == TRUE)
    {
        global $atrHp_array;
        $atrHp_array = array();
    }

    $GLOBALS['first_time_table4'] = FALSE;

}

//PRI ZADANI PARAMETRU --etc=n BOLO SKOPIROVANE VYSLEDNE POLE, PREHLADAVANA BOLA KOPIA, VYSLEDOK SA VZDY ULOZIL DO VYSLEDNEHO POLA
function N_array()
{
    if($GLOBALS['first_time_N'] == TRUE)
    {
        global $N_pole;
        $N_pole = array();
        $N_pole = array_merge($N_pole, $GLOBALS['element_array']);
    }

    $GLOBALS['first_time_N'] = FALSE;
}

//POMOCNE POLE PRE ATRIBUTY, VSETKY SA SPOJA DO HLAVNEHO POLA
function attr_array($attr, $attrV)
{
    $attr = strtoupper($attr);
    if($GLOBALS['first_time_table2'] == TRUE)
    {
        global $attr_array;
        $attr_array = array();
    }
    $GLOBALS['first_time_table2'] = FALSE;

    $isInt =  filter_var($attrV, FILTER_VALIDATE_INT);
    $isBool = strtoupper($attrV);
    $isFloat = filter_var($attrV, FILTER_VALIDATE_FLOAT);
    if($isInt !== FALSE)
        $attrV = "INT";

    elseif($attrV == "1" || $attrV == "0" || $isBool == "TRUE" || $isBool == "FALSE")
        $attrV = "BIT";

    elseif($isFloat !== FALSE)
        $attrV = "FLOAT";

    else
    {
        $dlzka = strlen($attrV);
        $isFloat2 = $attrV[$dlzka-1];
         if($isFloat2 == 'l' || $isFloat2 == 'L' || $isFloat2 == 'f'
         || $isFloat2 == 'F' )
         {
            $isFloat3 = substr($attrV, 0, -1);
            $isFloat4 = filter_var($isFloat3, FILTER_VALIDATE_FLOAT);
            if($isFloat4 !== FALSE)
                $attrV = "FLOAT";
            else
                $attrV = "NVARCHAR";
         }
         else
             $attrV = "NVARCHAR";
    }

    atrHelp_array();

    $finalne = array($attr => $attrV);
    array_push($GLOBALS['atrHp_array'], (array)$finalne );

    $GLOBALS['attr_array'] = array_merge((array)$GLOBALS['attr_array'], $GLOBALS['atrHp_array']);
    unset($GLOBALS['atrHp_array']);
    $GLOBALS['atrHp_array'] = array();


    if($GLOBALS['param_a'] || $GLOBALS['param_g'])
    {
        unset($GLOBALS['attr_array']);
        $GLOBALS['attr_array'] = array();
    }
}

//POMOCNE POLE PRE TEXTOVE ELEMENTY, VSETKY SA SPOJA DO HLAVNEHO POLA
function textVal_array($value)
{
    if($GLOBALS['first_time_table3'] == TRUE)
    {
        global $textVal_array;
        $textVal_array = array();
    }
    $GLOBALS['first_time_table3'] = FALSE;

    $isInt =  filter_var($value, FILTER_VALIDATE_INT);
    $isBool = strtoupper($value);
    $isFloat = filter_var($value, FILTER_VALIDATE_FLOAT);
    if($isInt !== FALSE)
        $value = "INT";

    elseif($value == "1" || $value == "0" || $isBool == "TRUE" || $isBool == "FALSE")
        $value = "BIT";

    elseif($isFloat !== FALSE)
        $value = "FLOAT";

    else
    {
        $dlzka = strlen($value);
        $isFloat2 = $value[$dlzka-1];
        if($isFloat2 == 'l' || $isFloat2 == 'L' || $isFloat2 == 'f'
            || $isFloat2 == 'F' )
        {
            $isFloat3 = substr($value, 0, -1);
            $isFloat4 = filter_var($isFloat3, FILTER_VALIDATE_FLOAT);
            if($isFloat4 !== FALSE)
                $value = "FLOAT";
            else
                $value = "NTEXT";
        }
        else
            $value = "NTEXT";
    }
    array_push($GLOBALS['textVal_array'], $value);
    $GLOBALS['attr_array'] = array_merge((array)$GLOBALS['textVal_array'] ,(array)$GLOBALS['attr_array'] );

    unset($GLOBALS['atrHp_array']);
    $GLOBALS['atrHp_array'] = array();

     unset($GLOBALS['textVal_array']);
     $GLOBALS['textVal_array'] = array();

}

//FUNKCIA PRE orezanie mena podelementu, koncova identifikacia _cislo kde cislo >=0
function removeID($key)
{
    $temp = $key;
    while(substr($temp, -1) != "_")
        $temp = substr($temp, 0, -1);

    $temp = substr($temp, 0, -1);
    return strtolower($temp);
}

//FUNKCIA PRE SPRACOVANIE CELEHO XML SUBORU, REKURZIVNE BEZI POSTUPNE NAD KAZDYM POTOMKOM (podelementom,ak ma)
function children_processing($child)
{
    //spracovanie textoveho elementu, ulozenie do pomocneho pola a neskor jeho spojenie do hlavneho pola
    foreach($child->children() as $more)
    {
        $value = trim((string)$more);
        if(strlen($value) > 0)
            textVal_array($value);
    //spracovanie atributov, pomocne pole sa kazdou iteraciou vyprazdni, aby bol v kazdom potomkovi aktualny atribut
        if($more->children() && !$more->attributes())
            if($GLOBALS['fix_bug_attr'])
            {
                unset($GLOBALS['attr_array']);
                $GLOBALS['attr_array']=array();
            }
            $GLOBALS['fix_bug_attr'] = FALSE;

       foreach($more->attributes() as $attr => $attrV)
            attr_array($attr , (string)$attrV);
    //do pomocneho pola pre atributy vlozime PODELEMENTY a konkatenujeme retazec indexom, aby sa vedelo urcit ktory kam patri a kolko ich je
       foreach($more->children() as $sub)
       {
           $final = strtolower($sub->getName());
           array_push($GLOBALS['attr_array'], $final . "_". $GLOBALS['index']);
       }
    //hlavne pole
       elements_array($more->getName());

    //ak ma element potomka, rekurzivne vyvolaj tuto funkciu nad nim, zvys index - identifikacia podelementu
        if($more->children())
        {
            $GLOBALS['index']++;
            children_processing($more);
        }
    }
}

//SPRACOVANIE VSTUPU, AK NIEJE ZADANY PARAMETER "--input=" tak sa nacita z STDIN
function input()
{
    if($GLOBALS['filename_in'] == NULL)
    {
        $filePtr_in = STDIN;
        $file_in_str = file_get_contents("php://stdin");
    }
    else
    {
        $filePtr_in = fopen($GLOBALS['filename_in'], "r");
        $file_in_str = fread($filePtr_in, filesize($GLOBALS['filename_in']));
    }

    if($filePtr_in == FALSE)
        return 2;

    $xml = simplexml_load_string($file_in_str);
    if($xml->children() == FALSE)
        exit(0);

    if($xml == FALSE)
        return 4;

    children_processing($xml);

    if($filePtr_in != STDIN)
        if(fclose($filePtr_in) == FALSE)
            return 2;

    return 0;
}

//SPRACOVANIE VYSTUPU, AK NIEJE ZADANY PARAMETER "--output=" tak sa vypise do STDOUT
function output()
{
    if($GLOBALS['filename_out'] == NULL)
        $GLOBALS['file_out'] = STDOUT;
    else
        $GLOBALS['file_out'] = fopen($GLOBALS['filename_out'], "w");

    if($GLOBALS['file_out'] == FALSE)
        return 3;

    return 0;
}

//Funkcia pre zatvorenie vystupneho suboru
function close()
{   //zatvorenie suboru pre vypis, ak bol zadany parameter --output=
    if(($GLOBALS['file_out'] != STDOUT) || ($GLOBALS['file_out'] != NULL))
        if(fclose($GLOBALS['file_out']) == FALSE)
            err_msg(3);
}

//TYP S VACSOU HODNOTOU BUDE VYPISANY
function solve_conflict_text($key)
{
    $tmp1 = 0;
    $tmp2 = 0;
    $tmp3 = 0;
    $tmp4 = 0;
    @$tmp5 = 0;
    $int = 0;
    //PRI VIACERYCH TEXTOVYCH PODELEMENTOCH A ROZNYCH TYPOCH SA VYPISE TEN KTORY JE NAJVIAC NAVRCHU TOHTO SWITCHU,PODLA ZADANIA
    foreach((array)$GLOBALS['element_array'][$key] as $kluc => $value)
    {
        if(is_int($kluc))
        {
        switch($GLOBALS['element_array'][$key][$kluc])
        {
            case "NTEXT":
                $tmp1 = $GLOBALS['element_array'][$key][$kluc];
                fputs($GLOBALS['file_out'], ",\n   value " . $tmp1);
                $int = 1;
                break;
            case "NVARCHAR":
                $tmp2 = $GLOBALS['element_array'][$key][$kluc];
                if($tmp1 == 0)
                {
                    fputs($GLOBALS['file_out'], ",\n   value " . $tmp2);
                    $int = 1;
                }
                break;
            case "FLOAT":
                $tmp3 = $GLOBALS['element_array'][$key][$kluc];
                if($tmp2 == 0)
                {
                    fputs($GLOBALS['file_out'], ",\n   value " . $tmp3);
                    $int = 1;
                }
                break;
            case "INT":
                $tmp4 = $GLOBALS['element_array'][$key][$kluc];
                if($tmp3 == 0)
                {
                    fputs($GLOBALS['file_out'], ",\n   value " . $tmp4);
                    $int = 1;
                }
                break;
            case "BIT":
                $tmp5 = $GLOBALS['element_array'][$key][$kluc];
                if($tmp4 == 0)
                {
                    fputs($GLOBALS['file_out'], ",\n   value " . $tmp5);
                    $int = 1;
                }
                break;
        }
        if($int) // AK SME ZISTILI NAJPRIORITNEJSI,SKONCIME
            break;
        }
    }
}

//TYP S VACSOU HODNOTOU BUDE VYPISANY
function solve_conflict_att($key)
{
    global $tmp_pole;
    $tmp_pole = array();
//PRI VIACERYCH ATRIBUTOCH ROVNAKEHO NAZVU ALE ROZNYCH TYPOCH SA VYPISE TEN KTORY JE NAJPRIORITNEJSI,PODLA ZADANIA
    foreach((array)$GLOBALS['element_array'][$key] as $kluc => $value)
        if(is_array($value))
            foreach((array)$value as $k1 => $k2)
            {
                  if($k2 == "BIT" || $k2 == "INT" || $k2 == "FLOAT"
                    || $k2 == "NVARCHAR")
                  {
                        $k1 = strtoupper($k1);
                        array_push($tmp_pole, $k1);
                        $tmp_pole[$k1] = $k2;

                        foreach((array)$tmp_pole as $cisla => $not)
                            if(is_int($cisla))
                                unset($tmp_pole[$cisla]);
                  }
            }
        foreach((array)$tmp_pole as $a => $b)
            fputs($GLOBALS['file_out'], ",\n   " . $a . " " . $b);
        unset($tmp_pole);  //NA ZAC. KAZDEJ ITERACIE SA VYTVORI NOVE POLE, ALE PRE ISTOTU HO NA KONCI KAZDEJ VYMAZEME, A ZNOVU VYTVORIME
        $tmp_pole = array();
}

//funkcia ktora meni pocet podelementov, popripade informuje deti o svojich rodicoch podla podtu podelementov
function N_arr_modify()
{
    N_array(); //vytvorenie kopie hlavneho pola, pri zadani parametru etc ho budeme potrebovat
    foreach((array)$GLOBALS['element_array'] as $key0 => $value0)
    //v cykle sa upravuje nad kazdym elementom kopirovaneho pola, a pridava sa do vysledneho pola
    //Dovod: pridava a kontroluje sa za behu, keby bolo iba 1 pole, tak by sa pridavali elementy viackrat (niektore)
    foreach($value0 as $kluc => $value)
    {
        if(is_array($value))
            foreach($value as $k1 => $k2)
            {
                if(is_int($k1))
                    if($GLOBALS['n'] == 0)// || $GLOBALS['param_g'])// ak je zadany parameter --etc=n a n=0
                    {
                        $tmp0 = strtolower($k2); // ZADANIE urcuje case in-sensitive, zmenil som na male pismena, aby sa lahsie hladali rovnake nazvy
                        $tmp2 = array("parent" => $key0);
                        array_push($GLOBALS['element_array'][$tmp0], $tmp2);
                        foreach((array)$GLOBALS['element_array'][$key0] as $cislo => $array)
                        {
                            if(is_array($array))
                                foreach((array)$array as $cis => $single_ele)
                                    if(is_int($cis))
                                        unset($GLOBALS['element_array'][$key0][$cislo]); // a pozera sa, ci sa tam nenachadza element jeden krat, ak ano, zmazeme ho
                        }
                    }

                if(is_int($k2))
                    if($GLOBALS['n'] >= 0)// || $GLOBALS['param_g']) // ak je zadany parameter --etc=n
                    {
                        if($GLOBALS['n'] < $k2 ) // ak je n vacsie nez pocet rovnakych podelementov
                        {
                            $GLOBALS['element_array'][$key0][$kluc][$k1] = null; // tak sa pocet rovnakych podelementov rovna nule, teda sa ziadny negeneruje
                            $GLOBALS['$N_pole'][$key0][$kluc][$k1] = null;
                            $tmp = removeID($k1);

                            foreach($GLOBALS['element_array'][$tmp] as $c => $arr)
                                if(is_array($arr))
                                    $tmp4 = array("parent" => $key0); //Do vysledneho pola sa ulozi pole n=>nazov, a podla klucu "parent" budeme hladat pri vypisovani
                            array_push($GLOBALS['element_array'][$tmp], $tmp4); //do vysledneho pola do dietata sa ulozi meno rodica
                        }
                    }
            }
    }
}

// VYPISE PODELEMENTY KAZDEHO ELEMENTU V HLAVNOM POLI
function solve_conflict_podelem($key)
{//Prehladavame cele pole, pre kazdy podelement
    foreach((array)$GLOBALS['element_array'][$key] as $kluc => $value)
    {
        if(is_array($value))
        foreach($value as $k1 => $k2)
        {
            if(is_int($k1)) //Vypisujeme podelement jedneho vyskytu
                fputs($GLOBALS['file_out'], ",\n   " . $k2 . "_id INT");

            if(is_int($k2))
                for($i=1; $i<= $k2; $i++)
                {
                    $final = removeID($k1);
                   //Vypisujeme podelement viacerych vyskytov, priradime im index podla poctu vyskytov
                    fputs($GLOBALS['file_out'], ",\n   " . $final .$i ."_id INT");
                }
            if($GLOBALS['n'] >= 0 && $k1 == "parent") //Ak je zadany parameter n, a nasli sme kluc "parent" v poli, tak vypiseme podlement
                fputs($GLOBALS['file_out'], ",\n   " . $k2 . "_id INT");

        }
    }
}

//funkcia pre poopravenie podelementov rovnakeho nazvu
function conflict_same_names()
{   //Funkcia, ktora zistuje chybu 90 - ak sa vyskytuje rovnaky nazov atributu_ID a podelementu
    foreach((array)$GLOBALS['element_array'] as $key => $hodnota)
    {
        foreach((array)$GLOBALS['element_array'][$key] as $kluc => $value)
        {
            if(is_array($value))
                foreach((array)$value as $k1 => $k2)
                {
                    if($k2 == "BIT" || $k2 == "INT" || $k2 == "FLOAT"
                        || $k2 == "NVARCHAR" || $k2 == "NTEXT") //Ak sa jedna o atribut,ktory je ulozeny vo vyslednom poli
                    {
                        if(substr($k1, -3, 1) == "_")
                        {
                           $final = removeID($k1);
                          //Podla zadania Case In-sensitive, zmenime na pismena velkej abecedy a prehladavame

                            foreach($GLOBALS['element_array'][$key] as $f1 => $f2)
                                foreach($f2 as $err)
                                    if($final == $err)
                                        err_msg(90); //v pripade zhody program ukoncime s navratovym kodom 90
                        }
                    }
                }
        }
    }
}

//mierna uprava vysledneho pola - zistime si pocet podelementov rovnakych nazvov, potom nemusime, staci vypisat
function correct_final_array()
{
    global $tmp_pole2;
    $tmp_pole2 = array(); //vytvorime si pomocne pole
    $pocet_elementov = 0; //Na zaciatku je pocet elementov rovnakeho nazvu 0

    foreach((array)$GLOBALS['element_array'] as $key => $value) //Prechadzame cele pole a vsetky podelementy
    {
        $pocet_prvkov = count($GLOBALS['element_array'][$key]);
        for($i=0; $i < $pocet_prvkov; $i++)
        {   //Ak sa nejedna o atribut, ani o textovy podelement tak pokracujeme, inak preskocime
            if(!($GLOBALS['element_array'][$key][$i] == "BIT" || $GLOBALS['element_array'][$key][$i] == "INT"
              || $GLOBALS['element_array'][$key][$i] == "FLOAT" || $GLOBALS['element_array'][$key][$i] == "NVARCHAR"
              || $GLOBALS['element_array'][$key][$i] == "NTEXT" || is_array($GLOBALS['element_array'][$key][$i])))
            {
                for($j=0; $j < $pocet_prvkov; $j++)
                {
                    $prvy = $GLOBALS['element_array'][$key][$i];
                    $druhy = $GLOBALS['element_array'][$key][$j];
                         if($prvy == $druhy) //Prehladavame kazdy s kazdym, ak najdeme rovnake, inkrementujeme pocitadlo
                            $pocet_elementov++;
                }
                $tmp = $GLOBALS['element_array'][$key][$i];

                if(in_array($tmp, $tmp_pole2));
                    array_push($tmp_pole2, $tmp); //Ulozime si nazov do pomocneho pola

                if($GLOBALS['param_b'])
                    $pocet_elementov = 1; //Ak je zadany parameter b, tak neskor vypiseme iba 1 podelement

                $tmp_pole2[$GLOBALS['element_array'][$key][$i]] = $pocet_elementov; //do pomocneho pola su ulozime aj pocet rovnakych
                $pocet_elementov = 0; //Vynulujeme, pre dalsiu iteraciu - dalsi podelement
            }
        }
        $tmp_pole2 = array_unique($tmp_pole2); //Zmazeme duplikovane udaje, struktura pola: nazov => pocet
        foreach((array)$tmp_pole2 as $kl => $vl)
            if(is_int($kl) || $tmp_pole2[$kl] == 1)
                unset($tmp_pole2[$kl]); //Zmazeme aj nepotrebne elementy ktore su tam raz

        foreach((array)$GLOBALS['element_array'][$key] as $kl2 => $vl2) //zistujeme ci kluc existuje, ak ano pridame do pomocneho pola
            if(!(is_array($vl2) || $vl2 == "BIT" || $vl2 == "TNT" || $vl2 == "FLOAT" || $vl2 == "NVARCHAR" || $vl2 == "NTEXT"))
                if(!array_key_exists($vl2, $tmp_pole2))
                    array_push($tmp_pole2, $vl2);

       foreach((array)$GLOBALS['element_array'][$key] as $kl3 => $vl3)
            if(!(is_array($vl3) || $vl3 == "BIT" || $vl3 == "TNT" || $vl3 == "FLOAT" || $vl3 == "NVARCHAR" || $vl3 == "NTEXT"))
                unset($GLOBALS['element_array'][$key][$kl3]);

        foreach((array)$tmp_pole2 as $kl4 => $vl4) //prechadzame pomocnym polom, mazeme duplikovane udaje
        {
            if(is_int($kl4))
            {
                while(substr($tmp_pole2[$kl4], -1) != "_")
                    $tmp_pole2[$kl4] = substr($tmp_pole2[$kl4], 0, -1);
                if(substr($tmp_pole2[$kl4], -1) == "_")
                    $tmp_pole2[$kl4] = substr($tmp_pole2[$kl4], 0, -1);
            }
        }
        $tmp_pole2 = array_unique($tmp_pole2);

        if(!empty($tmp_pole2)) //Ak nieje pomocne pole prazdne, pridame ho do vysledneho pola, a takto mame podelementy aj s poctami predpripravene na vypis
            array_push($GLOBALS['element_array'][$key] , $tmp_pole2);

        unset($tmp_pole2); //Pomocne pole na konci iteracie vynulujeme, vytvorime nove, aby sme mali vzdy aktualne udaje
        $tmp_pole2 = array();
        }
}

//PODELEMENT SA NEMOZE VYPISOVAT VIAC KRAT AK JE V ROVNAKOM NAZVE RODICA ALE NA INOM MIESTE
function correct2_podelem()
{   //Prehladavame kazdy podelement, a hladame ci niekde nieje samostatny podelement, ale taky, ktory je uz v rodici zastupeny(aj viackrat). Ak je, vymazeme ho.
    foreach((array)$GLOBALS['element_array'] as $key => $value)
    {
        foreach($value as $k1 => $v1)
            if(is_array($v1))
                foreach($v1 as $k2 => $v2)
                    if(is_int($v2))
                    {
                       $k2 = removeID($k2);

                        if(in_array($k2 , $v1))
                        {
                            $tmp = array_search($k2, $v1); //Ak najdeme, rusime,nech su podelementy na jednom a jednotnom mieste v poli
                            if($tmp != false)
                               unset($GLOBALS['element_array'][$key][$k1][$tmp]);
                        }
                    }
    }
}

//Tranzitivne iterujeme, maximalne tolko krat, kolko je podelementov s rovnakymi typmi kardinality
function transitive_iteration($ele, $parent1, $type)
{
    $iteruj = TRUE;
    while($iteruj) //iterujeme pokial sa v poli uz nenachadza ziadny typ kardinality, teda tranzitivita konci
        foreach((array)$GLOBALS['relation_array'][$ele] as $ele2 => $cis2)
        {
            if(!in_array($type,$GLOBALS['relation_array'][$ele])) $iteruj = FALSE; //podmienka ukoncenia tranzitivity
            if($ele2 != $parent1)
                if($cis2 == $type)
                {
                    $vztah = array($ele2 => $type); //umiestnime meno elementu a jeho vazbu s hlavnym elementom do pola
                    $GLOBALS['relation_array'][$parent1] = array_merge((array)$GLOBALS['relation_array'][$parent1], $vztah);
                }
            $ele = $ele2; // dalsiu iteraciu vykonavame nad potomkom, ktory ma rovnaky typ kardinality
        }               //ak je a->b a b->c , tak musi byt aj a->c, teda postupne prechadzame setky deti
}

function transitive_relation($type)
{
    foreach((array)$GLOBALS['relation_array'] as $parent1 => $arr1)
        foreach((array)$arr1 as $ele => $cis)
            if($parent1 != $ele)
                if($cis == $type)
                    transitive_iteration($ele, $parent1, $type); //zavolame funkciu, ktora bude urcovat kardinalitu
}

//funkcia ktora naplnuje pomocne pole typmi relacii
function get_relation()
{
      foreach((array)$GLOBALS['children_array'] as $parent1 => $arr1)
      {
            $vztah_1_1 = array($parent1 => 1); // pre vztah 1:1
            $GLOBALS['relation_array'][$parent1] = array_merge((array)$GLOBALS['relation_array'][$parent1], $vztah_1_1);
            foreach((array)$arr1 as $cislo_indexu => $child)
            {
                $vztah_1_1 = array($child => 1);// pre vztah 1:1
                $GLOBALS['relation_array'][$child] = array_merge((array)$GLOBALS['relation_array'][$child], $vztah_1_1);

                $vztah_N_1 = array($child => 2); //pre vztah N:1
                $vztah_1_N = array($parent1 => 3); //pre vztah N:1
                $GLOBALS['relation_array'][$parent1] = array_merge((array)$GLOBALS['relation_array'][$parent1], $vztah_N_1); //ulozime do pola
                $GLOBALS['relation_array'][$child] = array_merge((array)$GLOBALS['relation_array'][$child], $vztah_1_N);
                foreach((array)$arr1 as $cislo_indexu2 => $child2)
                    if($child != $child2)
                    {
                        $vztah_N_M = array($child => 4); //pre vztah N:M
                        $GLOBALS['relation_array'][$child2] = array_merge((array)$GLOBALS['relation_array'][$child2], $vztah_N_M);
                    }
            }
      }
     for($i = 2; $i <= 4; $i++) //iterujeme od 2 po 4, pretoze tolko typov kardinality mame
        transitive_relation($i);
}

//FUNKCIA PRE VYPIS Z POLA.V HLAVNOM POLI JE ULOZENE VSETKO- ELEMENTY, DETI, ATRIBUTY, ICH TYPY, AJ TYPY TEXTOVYCH ELEMENTOV
function print_elements()
{
    if($GLOBALS['header'] != NULL)
        fputs($GLOBALS['file_out'], $GLOBALS['header']);

    correct2_podelem(); //OPRAVA JEMNEHO BUGU, PODELEMENT SA NEMOZE VYPISOVAT VIAC KRAT AK JE V ROVNAKOM NAZVE RODICA ALE NA INOM MIESTE
    if($GLOBALS['n'] >= 0)
        N_arr_modify(); //funkcia ktora meni pocet podelementov, popripade informuje deti o svojich rodicoch podla podtu podelementov

    if($GLOBALS['param_g'] == FALSE)
        foreach((array)$GLOBALS['element_array'] as $key => $value) //bez parametra -g
        {
            fputs($GLOBALS['file_out'], "CREATE TABLE " . $key ."(\n");
            fputs($GLOBALS['file_out'], "   PRK_" .$key ."_ID INT PRIMARY KEY");
            solve_conflict_text($key);    //TYP S VACSOU HODNOTOU BUDE VYPISANY
            solve_conflict_podelem($key); //VYPISE PODELEMENTY KAZDEHO ELEMENTU V HLAVNOM POLI
            solve_conflict_att($key);     //TYP S VACSOU HODNOTOU BUDE VYPISANY
            fputs($GLOBALS['file_out'], "\n);\n\n");
        }
    else //s parametrom -g
    {
        arr_childrens(); //vytvorime si pomocne pole, v ktorom budu elementy a ich podelementy(netextove)
        arr_relation();  //vytvorime si pomocne pole, v ktorom budu neskor vztahy, vytvarat ich budeme podla pomocneho pola children_array, v kt. su potomkovia
        fputs($GLOBALS['file_out'], "<tables>" . "\n");
        get_relation(); //pole naplnime vztahmi
        foreach($GLOBALS['relation_array'] as $key => $array) //pre kazdy podelement vygenerujeme stlpec urcujuci vztah
            if(!empty($GLOBALS['relation_array'][$key]))
            {
                fputs($GLOBALS['file_out'], "\t<table name=\"" . $key. "\">" . "\n");
                foreach((array)$array as $key2 => $value2)
                {
                    if($value2 == 1)     $value2 = "1:1";
                    elseif($value2 == 2) $value2 = "N:1";
                    elseif($value2 == 3) $value2 = "1:N";
                    elseif($value2 == 4) $value2 = "N:M";

                    fputs($GLOBALS['file_out'], "\t\t<relation to=\"" . $key2 . "\" relation_type=\"" . $value2 . "\" />\n");
                }
                fputs($GLOBALS['file_out'], "\t</table>\n");
            }
        fputs($GLOBALS['file_out'], "</tables>");
    }
}
$return_code_param = params($argv, $argc); //sprac. parametrov
if($return_code_param != 0)
    err_msg($return_code_param);

$return_code_input = input(); //sprac. vstupu
if($return_code_input != 0)
    err_msg($return_code_input);

$return_code_output = output();//sprac. vystupu
if($return_code_output != 0)
    err_msg($return_code_output);

correct_final_array(); //mierna uprava vysledneho pola
conflict_same_names(); //funkcia pre poopravenie podelementov rovnakeho nazvu
print_elements(); // funkcia pre vypisovanie z pola
close(); //zatvorenie suboru pre vypis
?>