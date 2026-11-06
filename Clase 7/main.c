#include <stdio.h>

int main(void)
{
    double km, consumo, precio;
    double vida_neumaticos, costo_neumaticos;
    double intervalo_service, costo_service;
    double seguro_anual, km_anual;
    double combustible, neumaticos, service, fijos;
    double total, costo_km;

    printf("Kilometros recorridos: ");
    scanf("%lf", &km);

    printf("Consumo (litros/100 km): ");
    scanf("%lf", &consumo);

    printf("Precio del combustible por litro: ");
    scanf("%lf", &precio);

    printf("Vida util de los neumaticos (km): ");
    scanf("%lf", &vida_neumaticos);

    printf("Costo de los neumaticos: ");
    scanf("%lf", &costo_neumaticos);

    printf("Intervalo de service (km): ");
    scanf("%lf", &intervalo_service);

    printf("Costo del service: ");
    scanf("%lf", &costo_service);

    printf("Seguro anual: ");
    scanf("%lf", &seguro_anual);

    printf("Kilometros anuales estimados: ");
    scanf("%lf", &km_anual);

    combustible = (km / 100) * consumo * precio;
    neumaticos = (km / vida_neumaticos) * costo_neumaticos;
    service = (km / intervalo_service) * costo_service;
    fijos = (seguro_anual / km_anual) * km;

    total = combustible + neumaticos + service + fijos;
    costo_km = total / km;

    printf("\n=== Resultados ===\n");
    printf("Combustible: %.2f\n", combustible);
    printf("Neumaticos: %.2f\n", neumaticos);
    printf("Service: %.2f\n", service);
    printf("Costos fijos: %.2f\n", fijos);
    printf("Costo total: %.2f\n", total);
    printf("Costo por km: %.4f\n", costo_km);

    return 0;
}
