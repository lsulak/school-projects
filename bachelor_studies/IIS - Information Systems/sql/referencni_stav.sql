DROP TABLE IF EXISTS `rezervace`;
DROP TABLE IF EXISTS `vypujcka`;
DROP TABLE IF EXISTS `vytisk`;
DROP TABLE IF EXISTS `autor_titul`;
DROP TABLE IF EXISTS `zamestnanec`;
DROP TABLE IF EXISTS `autor`;
DROP TABLE IF EXISTS `ctenar`;
DROP TABLE IF EXISTS `titul`;
CREATE TABLE IF NOT EXISTS `autor` (
`id` int(11) NOT NULL,
  `jmeno` varchar(50) COLLATE utf8_unicode_ci NOT NULL,
  `prijmeni` varchar(50) COLLATE utf8_unicode_ci NOT NULL,
  `datum_narozeni` int(8) NOT NULL DEFAULT '0',
  `datum_umrti` int(8) NOT NULL DEFAULT '0'
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=18 ;
INSERT INTO `autor` (`id`, `jmeno`, `prijmeni`, `datum_narozeni`, `datum_umrti`) VALUES
(1, 'Edgar Allan', 'Poe', 19011809, 7101849),
(2, 'George', 'Orwell', 25061903, 21011950),
(4, 'Jo', 'Nesbø', 29031960, 0),
(5, 'Arthur Charles', 'Clarke', 16121917, 18032008),
(13, 'Ladislav', 'Smoljak', 9121931, 6062010),
(14, 'Zdeněk', 'Svěrák', 28031936, 0),
(15, 'Franz', 'Kafka', 3071883, 3061924),
(16, 'Mario', 'Puzo', 15101920, 2071999),
(17, 'Ken', 'Kesey', 17091935, 10112001);
CREATE TABLE IF NOT EXISTS `ctenar` (
`id` int(11) NOT NULL,
  `login` varchar(32) COLLATE utf8_unicode_ci NOT NULL,
  `heslo` varchar(200) COLLATE utf8_unicode_ci NOT NULL,
  `jmeno` varchar(50) COLLATE utf8_unicode_ci NOT NULL,
  `prijmeni` varchar(50) COLLATE utf8_unicode_ci NOT NULL,
  `adresa` varchar(120) COLLATE utf8_unicode_ci NOT NULL,
  `email` varchar(50) COLLATE utf8_unicode_ci NOT NULL,
  `telefon` varchar(15) COLLATE utf8_unicode_ci NOT NULL,
  `cas_vytvoreni` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=25 ;
INSERT INTO `ctenar` (`id`, `login`, `heslo`, `jmeno`, `prijmeni`, `adresa`, `email`, `telefon`, `cas_vytvoreni`) VALUES
(3, 'pesvan', '5ef724496ea2623e7dcc3b325c581960735f862e605165679d076535f1423ce4c8e55e49cf30792cd1f5aa776973b79f9211657f7de8e748f1891822243d3c99', 'Petr', 'Švaňa', 'Válcovní 30, Ostrava', 'pesvan2000@gmail.com', '+420731118013', '2014-10-23 10:17:57'),
(8, 'asdasd', 'ba3253876aed6bc22d4a6ff53d8406c6ad864195ed144ab5c87621b6c233b548baeae6956df346ec8c17f5ea10f35ee3cbc514797ed7ddd3145464e2a0bab413', 'laco', 'super', '', 'laco.sulak@azyt.sk', '', '2014-10-25 18:12:01'),
(10, 'miska', '54c34e430d23f16492d6ccd3b1c2ea3b1c2f573ef011a9774fe93f59890bf58d5fb824d393929bca1a5962504faba7a89da14b5190ac5983abf7a48176f6a646', 'miska', 'miska', '', 'simondelova@gmail.com', '', '2014-10-26 16:20:38'),
(11, 'exile', '52b7bffd00419df6ba18aaef350a665496e73d691de6cd3b705b3fc1ad86bf4d2b43804410cd05fca90fd95d4379e26509692fff88edf9c91414d751921978e6', 'Petr', 'Svana', '', 'petr.svana@gmail.com', '', '2014-10-28 10:02:37'),
(12, 'papapa', 'da1e949021f26c17be958caf31133a77ca48474ebbc1a127577794bb31b2674ef3bd4d4e6432ed72477948ac7bb51f439d53ba5723039cb44fc39b9a8cd0a216', 'Mam rad', 'Knihy', '', 'pep@pep.cs', '', '2014-10-28 15:32:47'),
(13, 'David', '37d1071f647fb81a4662768752122f1c784f6578dd9999bcece3f17e2014e112dd9d5d26c2cedce3b1257792c736b09a92d81b982330c6e0ced8ef0bad59a27e', 'David', 'Nekdo', 'internet 1337', 'davidskacel@seznam.cz', '123456789', '2014-11-01 13:40:29'),
(14, 'asddd', 'ba3253876aed6bc22d4a6ff53d8406c6ad864195ed144ab5c87621b6c233b548baeae6956df346ec8c17f5ea10f35ee3cbc514797ed7ddd3145464e2a0bab413', 'Vojta', 'Akcim', '', 'mikerustsy@azet.sk', '', '2014-11-01 13:41:50'),
(15, '123456', 'ba3253876aed6bc22d4a6ff53d8406c6ad864195ed144ab5c87621b6c233b548baeae6956df346ec8c17f5ea10f35ee3cbc514797ed7ddd3145464e2a0bab413', 'Radim', 'Kecavs', '', 'mikerussssty@azet.sk', '', '2014-11-01 13:44:25'),
(16, 'aaaaaaaaaaa', '7a9b2a35095dcdfedfdf0ef810310b409e38c92c20cbd51088ea5e4bc4873bdacfeb29f14b7f2ed033d87fad00036da83d5c597a7e7429bc70cec378db4de6a6', 'Tomas', 'Novák', '', 'mikeruaasty@azet.sk', '', '2014-11-01 13:52:06'),
(17, 'aaaaaaaaaaav', '7a9b2a35095dcdfedfdf0ef810310b409e38c92c20cbd51088ea5e4bc4873bdacfeb29f14b7f2ed033d87fad00036da83d5c597a7e7429bc70cec378db4de6a6', 'Radek', 'Novák', '', 'mikeruaaasty@azet.sk', '', '2014-11-01 13:52:35'),
(18, 'asaaaaaaaaaav', 'b0412597dcea813655574dc54a5b74967cf85317f0332a2591be7953a016f8de56200eb37d5ba593b1e4aa27cea5ca27100f94dccd5b04bae5cadd4454dba67d', 'Ondřej', 'Novák', '', 'mikaeruaaasty@azet.sk', '', '2014-11-01 13:53:13'),
(19, 'asdqq', '533e4fac5a60a5ed7602f9b5f85b7841290fb8debcc38d8150a49b91316ae715edad4d4bb3762b197be4a5d6c66f8d7ee38d769b3a1956d4b24d8787e4fddf42', 'Marek', 'Novák', '', 'mikeaarusty@azet.skasddd', '', '2014-11-01 13:53:59'),
(20, 'tttttt', '9e863bf17b1a33d8035dc26d47d57ce89aea4a23ee061c78510d7903be40beca30d34c254ecdf10139e96b039f25b63dbd0146fae747517bc82f56182c17cdf7', 'Rupert', 'Novák', '', 'mikertttusty@azet.sk', '', '2014-11-01 13:55:14'),
(21, 'asdqweqweqwe', '0752d364e2f1a4fad9c5c3faf530c4d86b085bafa3ef6b565d912e07327dd5ac207c992bb77e06c615c9c1fa77d4eb7289e8de5fb953bdecaa8dd6cc2063aecd', 'Lucia', 'Krásná', '', 'adaaasd', '', '2014-11-01 13:55:42'),
(22, 'qqqqqqqqqq', '479dc5e1f6f76285a02ac8e27e9b8d1f7e5b83dd781b3e135431d23c268e7f7fc3c6eadf3f83c32fccbbcd94841e13ecd06989a64f9e1b2e7dfe1fa425867ecd', 'Pavel', 'Novák', '', 'eqweqweqwe', '', '2014-11-01 13:56:20'),
(23, 'asdddqqqqq', 'b2112aa73994bd7cbb07732326ee102d585e706bb2e4fb878df5b5706ea92522f67b9f9dbc0208e2bd5b0f9cb21221bfb970f36a63e27e1a128ebc44d1ea5976', 'Lukáš', 'Novák', '', 'asdasda', '', '2014-11-01 13:57:23'),
(24, 'ctenar', '7a49db0934666d6d1ea318f1764989dbcc1fe40139cfba81f6fbb765613b8b37ba2d165af51531d7e9d199c6cbee45da8913d90f509302f748bdaef0b54d822f', 'Josef', 'Novák', 'Božetěchova 2, Brno, 61200', 'nefunknci@email.czsk', '+420765341289', '2014-11-16 19:09:44');
CREATE TABLE IF NOT EXISTS `rezervace` (
`id` int(11) NOT NULL,
  `id_ctenar` int(11) NOT NULL COMMENT 'FK',
  `datum_rezervace` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `stav` int(1) NOT NULL,
  `id_titul` int(11) NOT NULL,
  `id_vytisk` int(11) NOT NULL
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=27 ;
CREATE TABLE IF NOT EXISTS `titul` (
`id` int(11) NOT NULL,
  `nazev` varchar(100) COLLATE utf8_unicode_ci NOT NULL,
  `rok_vydani` int(4) NOT NULL,
  `isbn` varchar(30) COLLATE utf8_unicode_ci NOT NULL,
  `nakladatelstvi` varchar(30) COLLATE utf8_unicode_ci NOT NULL,
  `cas` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=279 ;
INSERT INTO `titul` (`id`, `nazev`, `rok_vydani`, `isbn`, `nakladatelstvi`, `cas`) VALUES
(1, 'Světlo Země', 1955, '978-80-7384-142-3', 'Baronet', '2014-10-28 11:48:41'),
(2, 'Farma zvířat', 1945, '80-7299-075-6', 'Aurora', '2014-10-28 11:48:41'),
(4, 'Jáma a kyvadlo a jiné povídky', 1988, '80-7309-070-8', 'XYZ', '2014-10-28 11:48:41'),
(5, 'Havran', 1845, '978-80-86205-36-6', 'KGB', '2014-10-28 11:48:41'),
(6, 'Pentagram', 2003, '978-80-7473-033-7', 'Kniha Zlín', '2014-10-28 11:48:41'),
(269, 'České nebe', 2009, '978-80-7185-959-8', 'Paseka', '2014-11-16 20:09:59'),
(274, 'Lijavec', 2002, '978-80-7185-959-8', 'Paseka', '2014-11-16 20:47:23'),
(275, 'Proměna', 1915, '978-80-87222-25-6', 'B4U Publishing', '2014-11-16 20:51:59'),
(276, 'Kmotr', 1969, '978-80-242-3820-3', 'Knižní klub', '2014-11-16 20:53:22'),
(277, 'Vyhoďme ho z kola ven', 1963, '80-86938-41-7', 'Euromedia Group', '2014-11-16 20:56:49');
CREATE TABLE IF NOT EXISTS `vypujcka` (
`id` int(11) NOT NULL,
  `id_ctenar` int(11) NOT NULL COMMENT 'FK',
  `id_vytisk` int(11) NOT NULL COMMENT 'FK',
  `datum_vypujcky` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `datum_vraceni` int(8) NOT NULL DEFAULT '0',
  `stav` int(1) NOT NULL DEFAULT '1',
  `u_prodl` int(11) NOT NULL DEFAULT '0'
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=25 ;
CREATE TABLE IF NOT EXISTS `vytisk` (
`id` int(11) NOT NULL,
  `id_titul` int(11) NOT NULL COMMENT 'FK',
  `dostupnost` int(1) NOT NULL DEFAULT '1',
  `cas` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=46 ;
CREATE TABLE IF NOT EXISTS `zamestnanec` (
`id` int(11) NOT NULL,
  `login` varchar(50) COLLATE utf8_unicode_ci NOT NULL,
  `heslo` varchar(200) COLLATE utf8_unicode_ci NOT NULL,
  `jmeno` varchar(50) COLLATE utf8_unicode_ci NOT NULL,
  `prijmeni` varchar(50) COLLATE utf8_unicode_ci NOT NULL,
  `adresa` varchar(120) COLLATE utf8_unicode_ci NOT NULL,
  `telefon` varchar(13) COLLATE utf8_unicode_ci NOT NULL,
  `email` varchar(50) COLLATE utf8_unicode_ci NOT NULL,
  `cas_vytvoreni` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `prava` int(1) NOT NULL DEFAULT '0'
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=8 ;
INSERT INTO `zamestnanec` (`id`, `login`, `heslo`, `jmeno`, `prijmeni`, `adresa`, `telefon`, `email`, `cas_vytvoreni`, `prava`) VALUES
(1, 'pesva', '84cf053e6632218d6272041d59763ee52663c4edb3269e03a8e9225bd67619dfee653ba20e7b3b00adea47dd356abeb5dda85d3f7b5c43862d3d52dd0e25918a', 'Petr', 'Svana', 'Válcovní 30, Ostrava', '+420731118013', 'xsvana00@stud.fit.vutbr.cz', '2014-10-28 15:45:43', 1),
(4, 'admin', '7a49db0934666d6d1ea318f1764989dbcc1fe40139cfba81f6fbb765613b8b37ba2d165af51531d7e9d199c6cbee45da8913d90f509302f748bdaef0b54d822f', 'Laco', 'Sulak', '', '', '', '2014-11-02 00:28:17', 1),
(7, 'zamestnanec', '7a49db0934666d6d1ea318f1764989dbcc1fe40139cfba81f6fbb765613b8b37ba2d165af51531d7e9d199c6cbee45da8913d90f509302f748bdaef0b54d822f', 'Karolina', 'Pultova', '', '0902259084', 'fit@zamestnanec.sk', '2014-11-07 21:54:08', 0);
CREATE TABLE IF NOT EXISTS `autor_titul` (
`id` int(11) NOT NULL,
  `id_autor` int(11) NOT NULL COMMENT 'FK',
  `id_titul` int(11) NOT NULL COMMENT 'FK'
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=38 ;
INSERT INTO `autor_titul` (`id`, `id_autor`, `id_titul`) VALUES
(1, 1, 4),
(3, 2, 2),
(5, 4, 6),
(18, 5, 1),
(19, 1, 5),
(26, 14, 269),
(29, 13, 274),
(30, 14, 274),
(31, 15, 275),
(32, 16, 276),
(33, 17, 277);
INSERT INTO `vytisk` (`id`, `id_titul`, `dostupnost`, `cas`) VALUES
(3, 2, 1, '2014-11-02 19:03:39'),
(4, 2, 1, '2014-11-02 19:03:39'),
(5, 2, 1, '2014-11-02 19:03:39'),
(6, 5, 1, '2014-11-02 19:03:53'),
(7, 5, 1, '2014-11-02 19:03:53'),
(8, 5, 1, '2014-11-02 19:03:53'),
(9, 5, 1, '2014-11-02 19:03:53'),
(10, 5, 1, '2014-11-02 19:03:53'),
(11, 5, 1, '2014-11-02 19:03:53'),
(12, 5, 1, '2014-11-02 19:03:53'),
(13, 5, 1, '2014-11-02 19:03:53'),
(14, 5, 1, '2014-11-02 19:03:53'),
(15, 4, 1, '2014-11-02 19:04:02'),
(16, 4, 1, '2014-11-02 19:04:02'),
(17, 4, 1, '2014-11-02 19:04:02'),
(18, 4, 1, '2014-11-02 19:04:02'),
(21, 6, 1, '2014-11-02 19:04:29'),
(22, 6, 1, '2014-11-02 19:04:29'),
(23, 6, 1, '2014-11-02 19:04:29'),
(24, 1, 1, '2014-11-02 19:04:36'),
(27, 5, 1, '2014-11-15 00:51:22'),
(28, 5, 1, '2014-11-15 00:51:23'),
(29, 5, 1, '2014-11-15 00:51:23'),
(30, 5, 1, '2014-11-15 00:51:23'),
(31, 5, 1, '2014-11-15 00:51:23'),
(32, 5, 1, '2014-11-15 00:51:23'),
(33, 275, 1, '2014-11-16 21:13:27'),
(34, 275, 1, '2014-11-16 21:13:27'),
(35, 275, 1, '2014-11-16 21:13:27'),
(36, 274, 1, '2014-11-16 21:13:36'),
(37, 274, 1, '2014-11-16 21:13:36'),
(38, 274, 1, '2014-11-16 21:13:36'),
(39, 274, 1, '2014-11-16 21:13:36'),
(40, 274, 1, '2014-11-16 21:13:36'),
(41, 277, 1, '2014-11-16 21:13:42'),
(42, 276, 1, '2014-11-16 21:14:03'),
(43, 276, 1, '2014-11-16 21:14:03'),
(44, 276, 1, '2014-11-16 21:14:03'),
(45, 276, 1, '2014-11-16 21:14:03');
ALTER TABLE `autor`
 ADD PRIMARY KEY (`id`);
ALTER TABLE `autor_titul`
 ADD PRIMARY KEY (`id`), ADD KEY `id_autor` (`id_autor`), ADD KEY `id_titul` (`id_titul`);
ALTER TABLE `ctenar`
 ADD PRIMARY KEY (`id`), ADD UNIQUE KEY `login` (`login`), ADD UNIQUE KEY `email` (`email`);
ALTER TABLE `rezervace`
 ADD PRIMARY KEY (`id`), ADD KEY `id_ctenar` (`id_ctenar`), ADD KEY `id_vytisk` (`id_vytisk`), ADD KEY `rezervace_ibfk_3` (`id_titul`);
ALTER TABLE `titul`
 ADD PRIMARY KEY (`id`);
ALTER TABLE `vypujcka`
 ADD PRIMARY KEY (`id`), ADD UNIQUE KEY `uniq` (`id_vytisk`,`id_ctenar`), ADD KEY `id_ctenar` (`id_ctenar`);
ALTER TABLE `vytisk`
 ADD PRIMARY KEY (`id`), ADD KEY `id_titul` (`id_titul`);
ALTER TABLE `zamestnanec`
 ADD PRIMARY KEY (`id`), ADD UNIQUE KEY `login` (`login`), ADD UNIQUE KEY `email` (`email`);
ALTER TABLE `autor`
MODIFY `id` int(11) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=18;
ALTER TABLE `autor_titul`
MODIFY `id` int(11) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=38;
ALTER TABLE `ctenar`
MODIFY `id` int(11) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=25;
ALTER TABLE `rezervace`
MODIFY `id` int(11) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=27;
ALTER TABLE `titul`
MODIFY `id` int(11) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=279;
ALTER TABLE `vypujcka`
MODIFY `id` int(11) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=25;
ALTER TABLE `vytisk`
MODIFY `id` int(11) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=46;
ALTER TABLE `zamestnanec`
MODIFY `id` int(11) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=8;
ALTER TABLE `autor_titul`
ADD CONSTRAINT `autor_titul_ibfk_1` FOREIGN KEY (`id_autor`) REFERENCES `autor` (`id`),
ADD CONSTRAINT `autor_titul_ibfk_2` FOREIGN KEY (`id_titul`) REFERENCES `titul` (`id`);
ALTER TABLE `rezervace`
ADD CONSTRAINT `rezervace_ibfk_3` FOREIGN KEY (`id_titul`) REFERENCES `titul` (`id`),
ADD CONSTRAINT `rezervace_ibfk_1` FOREIGN KEY (`id_ctenar`) REFERENCES `ctenar` (`id`),
ADD CONSTRAINT `rezervace_ibfk_2` FOREIGN KEY (`id_vytisk`) REFERENCES `vytisk` (`id`);
ALTER TABLE `vypujcka`
ADD CONSTRAINT `vypujcka_ibfk_1` FOREIGN KEY (`id_ctenar`) REFERENCES `ctenar` (`id`),
ADD CONSTRAINT `vypujcka_ibfk_2` FOREIGN KEY (`id_vytisk`) REFERENCES `vytisk` (`id`);
ALTER TABLE `vytisk`
ADD CONSTRAINT `vytisk_ibfk_1` FOREIGN KEY (`id_titul`) REFERENCES `titul` (`id`);