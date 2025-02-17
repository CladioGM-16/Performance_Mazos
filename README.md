double calcularSpeedup(double tiempoSecuencial, double tiempoParalelo) {
    return tiempoSecuencial / tiempoParalelo;
}

double calcularEficiencia(double speedup, int numHilos) {
    return (speedup / numHilos) * 100;
}

double calcularThroughput(size_t numTareas, double tiempoTotal) {
    return numTareas / tiempoTotal;
}

double calcularLatencia(double tiempoTotal, size_t numTareas) {
    return tiempoTotal / numTareas;
}

double calcularSpeedupAmdahl(double P, double S) {
    return 1 / ((1 - P) + (P / S));
}
