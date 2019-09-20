<?php

	include_once 'dbh.php';

	if(isset($_POST['submit'])){
		$prenotazione = mysqli_real_escape_string($conn, $_POST['ID_Prenotazione']);
		$bevanda = mysqli_real_escape_string($conn, $_POST['ID_Bevanda']);
		$quantita = mysqli_real_escape_string($conn, $_POST['Quantita']);

		$sql = "INSERT INTO ordine_bev VALUES ($prenotazione, '$bevanda', $quantita);";
		mysqli_query($conn, $sql);

		header("Location: ../php/success.php");
	}
	else{
		header("Location: ../php/error.php");
	}

?>