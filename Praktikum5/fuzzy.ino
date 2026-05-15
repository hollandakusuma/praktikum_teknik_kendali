// MEMBERSHIP FUNCTION SEGITIGA
float segitiga(float x, float a, float b, float c) {
  // kondisi khusus sisi kiri
  if (a == b && x <= b)  return 1;

  // kondisi khusus sisi kanan
  if (b == c && x >= b) return 1;

  // di luar range
  if (x <= a || x >= c)   return 0;

  // titik puncak
  if (x == b) return 1;

  // sisi naik
  if (x > a && x < b) return (x - a) / (b - a);

  // sisi turun
  return (c - x) / (c - b);
}

// OUTPUT MEMBERSHIP FUNCTION
// kiri
float mf_kiri(float z) {
  return segitiga(z, 0,, 90);
}

// tengah
float mf_tengah(float z) {
  return segitiga(z, 45, , 135);
}

// kanan
float mf_kanan(float z) {
  return segitiga(z, 90, , 180);
}
