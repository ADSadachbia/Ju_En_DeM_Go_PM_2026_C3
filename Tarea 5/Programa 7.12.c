printf(”\nIngrese la cadena de caracteres (máximo 50 caracteres): ”);
gets(cad);
while(cad[i] != ‘\0’)
{
if (islower (cad[i]))
mi++;
else
if (isupper (cad[i]))
ma++;
i++;
}
printf(”\n\nNúmero de letras minúsculas: %d”, mi);
printf(”\nNúmero de letras mayúsculas: %d”, ma);
}
