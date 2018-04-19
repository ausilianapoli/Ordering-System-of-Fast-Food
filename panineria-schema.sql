
/*SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL';
*/

CREATE DATABASE IF NOT EXISTS panineria CHARACTER SET utf8 COLLATE utf8_general_ci; 

USE panineria;

--
-- Structure of Table 'bevanda'
--

CREATE TABLE IF NOT EXISTS bevanda(
	ID_Bevanda VARCHAR(8) PRIMARY KEY,
	Nome VARCHAR(32) DEFAULT NULL,
	Prezzo_EUR DECIMAL(15,2) DEFAULT 0
) ENGINE=InnoDB, DEFAULT CHARSET=utf8;

--
-- Structure of Table 'ingrediente'
--

CREATE TABLE IF NOT EXISTS ingrediente(
	ID_Ingrediente VARCHAR(8) PRIMARY KEY,
	Nome VARCHAR(32) DEFAULT NULL,
	Note VARCHAR(32) DEFAULT NULL
) ENGINE=InnoDB, DEFAULT CHARSET=utf8;

--
-- Structure of Table 'composizione'
--

CREATE TABLE IF NOT EXISTS composizione(
	ID_Panino VARCHAR(8),
	ID_Ingrediente VARCHAR(8),
	PRIMARY KEY(ID_Panino, ID_Ingrediente)
) ENGINE=InnoDB, DEFAULT CHARSET=utf8;


--
-- Structure of Table 'panino'
--

CREATE TABLE IF NOT EXISTS panino(
	ID_Panino VARCHAR(8) PRIMARY KEY,
	Nome VARCHAR(32) DEFAULT NULL,
	Prezzo_EUR DECIMAL(15,2) DEFAULT 0,
) ENGINE=InnoDB, DEFAULT CHARSET=utf8;

--
-- Structure of Table 'tavolo'
--

CREATE TABLE IF NOT EXISTS tavolo(
	ID_Tavolo VARCHAR(8) PRIMARY KEY
) ENGINE=InnoDB, DEFAULT CHARSET=utf8;

--
-- Structure of Table 'prenotazione'
--

CREATE TABLE IF NOT EXISTS prenotazione(
	ID_Prenotazione BIGINT(12) UNSIGNED AUTO_INCREMENT PRIMARY KEY,
	ID_Tavolo VARCHAR(8),
	Data_Ora TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
	tot_panini DECIMAL(15,2) DEFAULT 0,
	tot_bevande DECIMAL(15,2) DEFAULT 0,
	totale_EUR DECIMAL (15,2) AS (tot_panini+tot_bevande) STORED
) ENGINE=InnoDB, DEFAULT CHARSET=utf8;

--
-- Structure of Table 'ordine_pan'
--

CREATE TABLE IF NOT EXISTS ordine_pan(
	ID_Prenotazione BIGINT(12) UNSIGNED,
	ID_Panino VARCHAR(8) DEFAULT 'P00',
	Quantità BIGINT(12) UNSIGNED DEFAULT 1,
	note VARCHAR(32) DEFAULT NULL,
	PRIMARY KEY(ID_Prenotazione, ID_Panino)
) ENGINE=InnoDB, DEFAULT CHARSET=utf8;

--
-- Structure of Table 'ordine_bev'
--

CREATE TABLE IF NOT EXISTS ordine_bev(
	ID_Prenotazione BIGINT(12) UNSIGNED,
	ID_Bevanda VARCHAR(8) DEFAULT 'B00',
	Quantità BIGINT(12) UNSIGNED DEFAULT 1,
	PRIMARY KEY(ID_Prenotazione, ID_Bevanda)
) ENGINE=InnoDB, DEFAULT CHARSET=utf8;

--
-- Limits for Table 'composizione'
--

ALTER TABLE composizione
	ADD CONSTRAINT panino_fk FOREIGN KEY (ID_Panino) REFERENCES panino(ID_Panino) ON DELETE CASCADE ON UPDATE CASCADE,
	ADD CONSTRAINT ingrediente_fk FOREIGN KEY (ID_Ingrediente) REFERENCES ingrediente (ID_Ingrediente) ON DELETE RESTRICT ON UPDATE CASCADE;

--
-- Limits for Table 'prenotazione'
--

ALTER TABLE prenotazione
	ADD CONSTRAINT tavolo_fk FOREIGN KEY (ID_Tavolo) REFERENCES tavolo (ID_Tavolo) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Limits for Table 'ordine_bev'
--

ALTER TABLE ordine_bev
	ADD CONSTRAINT prenotazione_fk FOREIGN KEY (ID_Prenotazione) REFERENCES prenotazione (ID_Prenotazione) ON DELETE CASCADE ON UPDATE CASCADE,
	ADD CONSTRAINT bevanda_fk FOREIGN KEY (ID_Bevanda) REFERENCES bevanda (ID_Bevanda) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Limits for Table 'ordine_pan'
--

ALTER TABLE ordine_pan
	ADD CONSTRAINT prenotazione2_fk FOREIGN KEY (ID_Prenotazione) REFERENCES prenotazione (ID_Prenotazione) ON DELETE CASCADE ON UPDATE CASCADE,
	ADD CONSTRAINT panino2_fk FOREIGN KEY (ID_Panino) REFERENCES panino (ID_Panino) ON DELETE CASCADE ON UPDATE CASCADE;


--
-- Triggers for Table 'ordine_pan'
--

@delimiter %%%;

CREATE TRIGGER aggiornaTotPAN_Insert
AFTER INSERT ON ordine_pan
FOR EACH ROW

BEGIN
	DECLARE prezzo DECIMAL(15,2) DEFAULT 0;
	DECLARE totale DECIMAL(15,2) DEFAULT 0;
    DECLARE no_more_range INT UNSIGNED DEFAULT 0;

	DECLARE iterator CURSOR FOR
	SELECT p.prezzo_EUR*new.Quantità prezzo
	FROM panino p
	WHERE p.ID_panino=new.ID_Panino;

	DECLARE CONTINUE HANDLER FOR NOT FOUND SET no_more_range=1;

	OPEN iterator;
	cycle: LOOP
		FETCH iterator INTO prezzo;
		SET totale = totale + prezzo;
	        IF no_more_range=1 THEN LEAVE cycle; END IF;
	END LOOP cycle;
	CLOSE iterator;

	UPDATE prenotazione SET tot_panini = tot_panini + ((totale)/2)
	WHERE ID_Prenotazione=NEW.ID_Prenotazione;
END
%%%
@delimiter ;

---

@delimiter %%%;

CREATE TRIGGER aggiornaTotPAN_Delete
AFTER DELETE ON ordine_pan
FOR EACH ROW

BEGIN
	DECLARE prezzo DECIMAL(15,2) DEFAULT 0;
	DECLARE totale DECIMAL(15,2) DEFAULT 0;
    DECLARE no_more_range INT UNSIGNED DEFAULT 0;

	DECLARE iterator CURSOR FOR
	SELECT p.prezzo_EUR*old.Quantità prezzo
	FROM panino p
	WHERE p.ID_panino=old.ID_Panino;

	
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET no_more_range=1;

	OPEN iterator;
	cycle: LOOP
		FETCH iterator INTO prezzo;
		SET totale = totale + prezzo;
	        IF no_more_range=1 THEN LEAVE cycle; END IF;
	END LOOP cycle;
	CLOSE iterator;

	UPDATE prenotazione SET tot_panini = tot_panini - (totale)/2
	WHERE ID_Prenotazione=old.ID_Prenotazione;
END
%%%

@delimiter ;

---

@delimiter %%%;

CREATE TRIGGER aggiornaTotPAN_Update
AFTER UPDATE ON ordine_pan
FOR EACH ROW

BEGIN
	DECLARE prezzo DECIMAL(15,2) DEFAULT 0;
	DECLARE totale DECIMAL(15,2) DEFAULT 0;
	DECLARE prezzoOld DECIMAL (15,2) DEFAULT 0;
	DECLARE no_more_range INT UNSIGNED DEFAULT 0;

	DECLARE iterator CURSOR FOR
	SELECT p.prezzo_EUR*new.Quantità prezzo
	FROM Panino p
	WHERE p.ID_Panino=new.ID_Panino;

	DECLARE CONTINUE HANDLER FOR NOT FOUND SET no_more_range=1;

	OPEN iterator;
	cycle: LOOP
		FETCH iterator INTO prezzo;
		SET totale = totale + prezzo;
			IF no_more_range=1 THEN LEAVE cycle; END IF;
	END LOOP cycle;
	CLOSE iterator;

	SELECT p.prezzo_EUR*old.Quantità INTO prezzoOld
	FROM panino p
	WHERE p.ID_Panino=old.ID_Panino;

	UPDATE prenotazione SET tot_panini = tot_panini - prezzoOld + (totale)/2 
	WHERE ID_Prenotazione=new.ID_Prenotazione;
END
%%%

@delimiter ;

--
-- Triggers for Table 'ordine_bev'
--

@delimiter %%%;

CREATE TRIGGER aggiornaTotBEV_Insert
AFTER INSERT ON ordine_bev
FOR EACH ROW

BEGIN
	DECLARE prezzo DECIMAL(15,2) DEFAULT 0;
	DECLARE totale DECIMAL(15,2) DEFAULT 0;
    DECLARE no_more_range INT UNSIGNED DEFAULT 0;

	DECLARE iterator CURSOR FOR
	SELECT b.prezzo_EUR*new.Quantità prezzo
	FROM bevanda b
	WHERE b.ID_Bevanda=new.ID_Bevanda;

	DECLARE CONTINUE HANDLER FOR NOT FOUND SET no_more_range=1;

	OPEN iterator;
	cycle: LOOP
		FETCH iterator INTO prezzo;
		SET totale = totale + prezzo;
	        IF no_more_range=1 THEN LEAVE cycle; END IF;
	END LOOP cycle;
	CLOSE iterator;

	UPDATE prenotazione SET tot_bevande = tot_bevande + ((totale)/2)
	WHERE ID_Prenotazione=NEW.ID_Prenotazione;
END
%%%
@delimiter ;

---

@delimiter %%%;

CREATE TRIGGER aggiornaTotBEV_Delete
AFTER DELETE ON ordine_bev
FOR EACH ROW

BEGIN
	DECLARE prezzo DECIMAL(15,2) DEFAULT 0;
	DECLARE totale DECIMAL(15,2) DEFAULT 0;
    DECLARE no_more_range INT UNSIGNED DEFAULT 0;

	DECLARE iterator CURSOR FOR
	SELECT b.prezzo_EUR*old.Quantità prezzo
	FROM bevanda b
	WHERE b.ID_Bevanda=old.ID_Bevanda;

	
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET no_more_range=1;

	OPEN iterator;
	cycle: LOOP
		FETCH iterator INTO prezzo;
		SET totale = totale + prezzo;
	        IF no_more_range=1 THEN LEAVE cycle; END IF;
	END LOOP cycle;
	CLOSE iterator;

	UPDATE prenotazione SET tot_bevande = tot_bevande - (totale)/2
	WHERE ID_Prenotazione=old.ID_Prenotazione;
END
%%%

@delimiter ;
---

@delimiter %%%;

CREATE TRIGGER aggiornaTotBEV_Update
AFTER UPDATE ON ordine_bev
FOR EACH ROW

BEGIN
	DECLARE prezzo DECIMAL(15,2) DEFAULT 0;
	DECLARE totale DECIMAL(15,2) DEFAULT 0;
	DECLARE prezzoOld DECIMAL (15,2) DEFAULT 0;
	DECLARE no_more_range INT UNSIGNED DEFAULT 0;

	DECLARE iterator CURSOR FOR
	SELECT b.prezzo_EUR*new.Quantità prezzo
	FROM bevanda b
	WHERE b.ID_Bevanda=new.ID_Bevanda;

	DECLARE CONTINUE HANDLER FOR NOT FOUND SET no_more_range=1;

	OPEN iterator;
	cycle: LOOP
		FETCH iterator INTO prezzo;
		SET totale = totale + prezzo;
			IF no_more_range=1 THEN LEAVE cycle; END IF;
	END LOOP cycle;
	CLOSE iterator;

	SELECT b.prezzo_EUR*old.Quantità INTO prezzoOld
	FROM bevanda b
	WHERE b.ID_Bevanda=old.ID_Bevanda;

	UPDATE prenotazione SET tot_bevande = tot_bevande - prezzoOld + (totale)/2 
	WHERE ID_Prenotazione=new.ID_Prenotazione;
END
%%%

@delimiter ;
---


/*SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;*/