<?php

	include_once 'dbh.php';

	if(isset($_POST['submit'])){
		$bevanda = mysqli_real_escape_string($conn, $_POST['ID_Bevanda']);
		$nome = mysqli_real_escape_string($conn, $_POST['nome']);
		$prezzo = floatval(mysqli_real_escape_string($conn, $_POST['prezzo']));

		$sql = "INSERT INTO bevanda VALUES ('$bevanda','$nome','$prezzo');";
		mysqli_query($conn, $sql);

		header("Location: ../php/success.php");
	}
	else{
		header("Location: ../php/error.php");
	}

?>