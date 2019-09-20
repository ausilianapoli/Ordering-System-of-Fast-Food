<?php

	include_once 'dbh.php';

	if(isset($_POST['submit'])){
		$ingrediente = mysqli_real_escape_string($conn, $_POST['ID_Ingrediente']);
		$panino = mysqli_real_escape_string($conn, $_POST['ID_Panino']);

		$sql = "INSERT INTO composizione VALUES ('$panino', '$ingrediente');";
		mysqli_query($conn, $sql);

		header("Location: ../php/success.php");
	}
	else{
		header("Location: ../php/error.php");
	}

?>