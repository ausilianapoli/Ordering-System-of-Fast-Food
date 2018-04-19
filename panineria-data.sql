--
-- Data insert for table 'panino'
--

INSERT INTO panino VALUES ('P00', 'NESSUN PANINO', default), ('P01', 'Classic Burger', 4.00), ('P02', 'Manzo Vegeratiano', 4.50),
							('P03', 'Classic Pollo', 4.00), ('P04', 'Pollo Vegeratiano', 4.00), ('P05', 'Classic Cavallo', 4.00),
							('P06', 'Catanese', 4.00), ('P07', 'Cavallo Pazzo', 4.00), ('P08', 'BigOne', 5.00), 
							('P09', 'Kebab', 4.00), ('P10', 'Porketta', 4.50), ('P11', 'Porketta Plus', 4.50), ('P12', 'Burger Kids', 4.00),
							('P13', 'Hot Dog', 4.00); 


--
-- Data insert for table 'ingrediente'
--

INSERT INTO ingrediente VALUES ('I01', 'Hamburger Manzo', null), ('I02', 'Pomodoro', null), ('I03', 'Lattuga', null),
								('I04', 'Cheddar', null), ('I05', 'Bacon', null), ('I06', 'Salsa Hamburger', null), ('I07', 'Emmenthal', null),
								('I08', 'Melanzane grigliate', null), ('I09', 'Salsa BBQ', null), ('I10', 'Peperoni in agrodolce', null),
								('I11', 'Salsa Piccante', null), ('I12', 'Hamburger Pollo', null), ('I13', 'Cipolla saltata', null),
								('I14', 'Zucchina grigliata', null), ('I15', 'Provola affumicata', null), ('I16', 'Porchetta', null),
								('I17', 'Hamburger Cavallo', null), ('I18', 'Cipolla', null), ('I19', 'Salsa Yogurth', null),
								('I20', 'Mais', null), ('I21', 'Wurstel', null), ('I22', 'Patatine', 'prodotto surgelato'), 
								('I23', 'Kebab', 'prodotto surgelato'); 
 
 
--
-- Data insert for table 'composizione'
--

INSERT INTO composizione VALUES ('P01', 'I01'), ('P01', 'I02'), ('P01', 'I03'), ('P01', 'I04'), ('P01', 'I05'), ('P01', 'I06'), ('P01', 'I22'),
								('P02', 'I01'), ('P02', 'I02'), ('P02', 'I03'), ('P02', 'I07'), ('P02', 'I08'), ('P02', 'I22'), ('P03', 'I12'),
								('P03', 'I02'), ('P03', 'I03'), ('P03', 'I04'), ('P03', 'I10'), ('P03', 'I22'), ('P04', 'I12'), ('P04', 'I02'),
								('P04', 'I03'), ('P04', 'I04'), ('P04', 'I08'), ('P04', 'I14'), ('P05', 'I17'), ('P05', 'I02'), ('P05', 'I03'),
								('P05', 'I07'), ('P05', 'I09'), ('P06', 'I17'), ('P06', 'I02'), ('P06', 'I03'), ('P06', 'I13'), ('P06', 'I15'),
								('P07', 'I09'), ('P08', 'I01'), ('P08', 'I02'), ('P08', 'I03'), ('P08', 'I04'), ('P08', 'I10'),	('P08', 'I11'),
								('P09', 'I23'), ('P09', 'I03'), ('P09', 'I02'), ('P09', 'I20'), ('P09', 'I18'), ('P09', 'I22'), ('P09', 'I19'),
								('P10', 'I02'), ('P10', 'I03'), ('P10', 'I07'), ('P10', 'I09'), ('P10', 'I16'), ('P10', 'I22'), ('P11', 'I16'), 
								('P11', 'I03'), ('P11', 'I04'), ('P11', 'I08'), ('P11', 'I13'), ('P11', 'I06'), ('P11', 'I22'),
								('P11', 'I02'), ('P12', 'I22'), ('P12', 'I12'), ('P13', 'I21'), ('P13', 'I09'), ('P13', 'I22');
								

--
-- Data insert for table 'tavolo'
--

INSERT INTO tavolo VALUES ('T01'), ('T02'), ('T03'), ('T04'), ('T05'), ('T06'), ('T07'), ('T08'), ('T09'), ('T10'), ('T11'), ('T12');

--
-- Data insert for table 'bevanda'
--

INSERT INTO bevanda VALUES ('B00', 'NESSUNA BEVANDA', default), ('B01', 'Acqua Naturale 0.5 L', 0.50), ('B02', 'Acqua Naturale 1.5 L', 1.20), 
							('B03', 'Acqua Frizzante 0.5', 0.50), ('B04', 'Acqua Frizzante 1.5 L', 1.20), ('B05', 'Coca Cola 33 cl', 1.50),
							('B06', 'Fanta 33 cl', 1.50), ('B07', 'LemonSoda 33 cl', 1.50), ('B08', 'Birra 33 cl', 1.50);

--
-- Data insert for table 'prenotazione'
--

INSERT INTO prenotazione VALUES (default, 'T01', default, default, default, default), (default, 'T02', default, default, default, default),
								(default, 'T03', default, default, default, default), (default, 'T04', default, default, default, default),
								(default, 'T05', default, default, default, default), (default, 'T06', default, default, default, default),
								(default, 'T07', default, default, default, default), (default, 'T08', default, default, default, default),
								(default, 'T09', default, default, default, default), (default, 'T10', default, default, default, default),
								(default, 'T11', default, default, default, default), (default, 'T12', default, default, default, default);

--
-- Data insert for table 'ordine_pan'
--

INSERT INTO ordine_pan VALUES (1, 'P01', default, default), (2, 'P02', default, default), (3, 'P03', default, default),
							(4, 'P04', default, default), (5, 'P05', 2, default), (6, 'P06', 2, default), (7, 'P07', default, default),
							(8, 'P08', 3, default), (9, 'P09', default, default), (10, 'P10', 2, default), (10, 'P11', 4, default),
							(11, 'P11', 2, default), (12, 'P12', 3, default), (12, 'P13', default, default);

--
-- Data insert for table 'ordine_bev'
--

INSERT INTO ordine_bev VALUES (1, 'B01', 4), (2, 'B02', default), (3, 'B03', default), (4, 'B04', default), (5, 'B05', default), (6, 'B06', 2),
							(7, 'B07', 2), (8, 'B08', default), (9, 'B00', default), (10, 'B01', default), (10, 'B08', 2), (11, 'B01', 2),
							(12, 'B02', default), (12, 'B03', default);