const int Digits[10][10] =
{
  {11, 10, 8, 7, 18, 14, 24, 22}, //{7,8,10,11,14,18,22,24},
  {14,16,18,22,24}, // OK
  {11, 10, 9, 7, 18, 16, 14, 22}, //      {7,8,9,11,14,16,18,24},
  {7,9,11,14,16,18,22,24}, // OK
  {9, 8, 7, 16, 14, 24, 22}, //{9,10,11,16,18,22,24},
  {11, 9, 8, 7, 18, 16, 14, 24}, // {7,9,10,11,14,16,18,22},
  {11, 10, 9, 18, 16, 14, 24}, // {7,8,9,14,15,16,18,22},
  {11, 7, 18, 16, 15, 22}, // {7,11,14,16,17,24},
  {11, 10, 9, 8, 7, 18, 16, 14, 24, 22}, // {7,8,9,10,11,14,16,18,22,24},
  {11, 9, 8, 7, 18, 16, 15, 22}, // {7,9,10,11,14,16,17,24},
};