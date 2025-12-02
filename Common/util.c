

#include "util.h"
#include <math.h>

#define SIN_TABLE_SIZE  512

static const float sinTable_f32[SIN_TABLE_SIZE + 1] = {
   0.00000000f, 0.01227154f, 0.02454123f, 0.03680722f, 0.04906767f, 0.06132074f,
   0.07356456f, 0.08579731f, 0.09801714f, 0.11022221f, 0.12241068f, 0.13458071f,
   0.14673047f, 0.15885814f, 0.17096189f, 0.18303989f, 0.19509032f, 0.20711138f,
   0.21910124f, 0.23105811f, 0.24298018f, 0.25486566f, 0.26671276f, 0.27851969f,
   0.29028468f, 0.30200595f, 0.31368174f, 0.32531029f, 0.33688985f, 0.34841868f,
   0.35989504f, 0.37131719f, 0.38268343f, 0.39399204f, 0.40524131f, 0.41642956f,
   0.42755509f, 0.43861624f, 0.44961133f, 0.46053871f, 0.47139674f, 0.48218377f,
   0.49289819f, 0.50353838f, 0.51410274f, 0.52458968f, 0.53499762f, 0.54532499f,
   0.55557023f, 0.56573181f, 0.57580819f, 0.58579786f, 0.59569930f, 0.60551104f,
   0.61523159f, 0.62485949f, 0.63439328f, 0.64383154f, 0.65317284f, 0.66241578f,
   0.67155895f, 0.68060100f, 0.68954054f, 0.69837625f, 0.70710678f, 0.71573083f,
   0.72424708f, 0.73265427f, 0.74095113f, 0.74913639f, 0.75720885f, 0.76516727f,
   0.77301045f, 0.78073723f, 0.78834643f, 0.79583690f, 0.80320753f, 0.81045720f,
   0.81758481f, 0.82458930f, 0.83146961f, 0.83822471f, 0.84485357f, 0.85135519f,
   0.85772861f, 0.86397286f, 0.87008699f, 0.87607009f, 0.88192126f, 0.88763962f,
   0.89322430f, 0.89867447f, 0.90398929f, 0.90916798f, 0.91420976f, 0.91911385f,
   0.92387953f, 0.92850608f, 0.93299280f, 0.93733901f, 0.94154407f, 0.94560733f,
   0.94952818f, 0.95330604f, 0.95694034f, 0.96043052f, 0.96377607f, 0.96697647f,
   0.97003125f, 0.97293995f, 0.97570213f, 0.97831737f, 0.98078528f, 0.98310549f,
   0.98527764f, 0.98730142f, 0.98917651f, 0.99090264f, 0.99247953f, 0.99390697f,
   0.99518473f, 0.99631261f, 0.99729046f, 0.99811811f, 0.99879546f, 0.99932238f,
   0.99969882f, 0.99992470f, 1.00000000f, 0.99992470f, 0.99969882f, 0.99932238f,
   0.99879546f, 0.99811811f, 0.99729046f, 0.99631261f, 0.99518473f, 0.99390697f,
   0.99247953f, 0.99090264f, 0.98917651f, 0.98730142f, 0.98527764f, 0.98310549f,
   0.98078528f, 0.97831737f, 0.97570213f, 0.97293995f, 0.97003125f, 0.96697647f,
   0.96377607f, 0.96043052f, 0.95694034f, 0.95330604f, 0.94952818f, 0.94560733f,
   0.94154407f, 0.93733901f, 0.93299280f, 0.92850608f, 0.92387953f, 0.91911385f,
   0.91420976f, 0.90916798f, 0.90398929f, 0.89867447f, 0.89322430f, 0.88763962f,
   0.88192126f, 0.87607009f, 0.87008699f, 0.86397286f, 0.85772861f, 0.85135519f,
   0.84485357f, 0.83822471f, 0.83146961f, 0.82458930f, 0.81758481f, 0.81045720f,
   0.80320753f, 0.79583690f, 0.78834643f, 0.78073723f, 0.77301045f, 0.76516727f,
   0.75720885f, 0.74913639f, 0.74095113f, 0.73265427f, 0.72424708f, 0.71573083f,
   0.70710678f, 0.69837625f, 0.68954054f, 0.68060100f, 0.67155895f, 0.66241578f,
   0.65317284f, 0.64383154f, 0.63439328f, 0.62485949f, 0.61523159f, 0.60551104f,
   0.59569930f, 0.58579786f, 0.57580819f, 0.56573181f, 0.55557023f, 0.54532499f,
   0.53499762f, 0.52458968f, 0.51410274f, 0.50353838f, 0.49289819f, 0.48218377f,
   0.47139674f, 0.46053871f, 0.44961133f, 0.43861624f, 0.42755509f, 0.41642956f,
   0.40524131f, 0.39399204f, 0.38268343f, 0.37131719f, 0.35989504f, 0.34841868f,
   0.33688985f, 0.32531029f, 0.31368174f, 0.30200595f, 0.29028468f, 0.27851969f,
   0.26671276f, 0.25486566f, 0.24298018f, 0.23105811f, 0.21910124f, 0.20711138f,
   0.19509032f, 0.18303989f, 0.17096189f, 0.15885814f, 0.14673047f, 0.13458071f,
   0.12241068f, 0.11022221f, 0.09801714f, 0.08579731f, 0.07356456f, 0.06132074f,
   0.04906767f, 0.03680722f, 0.02454123f, 0.01227154f, 0.00000000f, -0.01227154f,
   -0.02454123f, -0.03680722f, -0.04906767f, -0.06132074f, -0.07356456f,
   -0.08579731f, -0.09801714f, -0.11022221f, -0.12241068f, -0.13458071f,
   -0.14673047f, -0.15885814f, -0.17096189f, -0.18303989f, -0.19509032f,
   -0.20711138f, -0.21910124f, -0.23105811f, -0.24298018f, -0.25486566f,
   -0.26671276f, -0.27851969f, -0.29028468f, -0.30200595f, -0.31368174f,
   -0.32531029f, -0.33688985f, -0.34841868f, -0.35989504f, -0.37131719f,
   -0.38268343f, -0.39399204f, -0.40524131f, -0.41642956f, -0.42755509f,
   -0.43861624f, -0.44961133f, -0.46053871f, -0.47139674f, -0.48218377f,
   -0.49289819f, -0.50353838f, -0.51410274f, -0.52458968f, -0.53499762f,
   -0.54532499f, -0.55557023f, -0.56573181f, -0.57580819f, -0.58579786f,
   -0.59569930f, -0.60551104f, -0.61523159f, -0.62485949f, -0.63439328f,
   -0.64383154f, -0.65317284f, -0.66241578f, -0.67155895f, -0.68060100f,
   -0.68954054f, -0.69837625f, -0.70710678f, -0.71573083f, -0.72424708f,
   -0.73265427f, -0.74095113f, -0.74913639f, -0.75720885f, -0.76516727f,
   -0.77301045f, -0.78073723f, -0.78834643f, -0.79583690f, -0.80320753f,
   -0.81045720f, -0.81758481f, -0.82458930f, -0.83146961f, -0.83822471f,
   -0.84485357f, -0.85135519f, -0.85772861f, -0.86397286f, -0.87008699f,
   -0.87607009f, -0.88192126f, -0.88763962f, -0.89322430f, -0.89867447f,
   -0.90398929f, -0.90916798f, -0.91420976f, -0.91911385f, -0.92387953f,
   -0.92850608f, -0.93299280f, -0.93733901f, -0.94154407f, -0.94560733f,
   -0.94952818f, -0.95330604f, -0.95694034f, -0.96043052f, -0.96377607f,
   -0.96697647f, -0.97003125f, -0.97293995f, -0.97570213f, -0.97831737f,
   -0.98078528f, -0.98310549f, -0.98527764f, -0.98730142f, -0.98917651f,
   -0.99090264f, -0.99247953f, -0.99390697f, -0.99518473f, -0.99631261f,
   -0.99729046f, -0.99811811f, -0.99879546f, -0.99932238f, -0.99969882f,
   -0.99992470f, -1.00000000f, -0.99992470f, -0.99969882f, -0.99932238f,
   -0.99879546f, -0.99811811f, -0.99729046f, -0.99631261f, -0.99518473f,
   -0.99390697f, -0.99247953f, -0.99090264f, -0.98917651f, -0.98730142f,
   -0.98527764f, -0.98310549f, -0.98078528f, -0.97831737f, -0.97570213f,
   -0.97293995f, -0.97003125f, -0.96697647f, -0.96377607f, -0.96043052f,
   -0.95694034f, -0.95330604f, -0.94952818f, -0.94560733f, -0.94154407f,
   -0.93733901f, -0.93299280f, -0.92850608f, -0.92387953f, -0.91911385f,
   -0.91420976f, -0.90916798f, -0.90398929f, -0.89867447f, -0.89322430f,
   -0.88763962f, -0.88192126f, -0.87607009f, -0.87008699f, -0.86397286f,
   -0.85772861f, -0.85135519f, -0.84485357f, -0.83822471f, -0.83146961f,
   -0.82458930f, -0.81758481f, -0.81045720f, -0.80320753f, -0.79583690f,
   -0.78834643f, -0.78073723f, -0.77301045f, -0.76516727f, -0.75720885f,
   -0.74913639f, -0.74095113f, -0.73265427f, -0.72424708f, -0.71573083f,
   -0.70710678f, -0.69837625f, -0.68954054f, -0.68060100f, -0.67155895f,
   -0.66241578f, -0.65317284f, -0.64383154f, -0.63439328f, -0.62485949f,
   -0.61523159f, -0.60551104f, -0.59569930f, -0.58579786f, -0.57580819f,
   -0.56573181f, -0.55557023f, -0.54532499f, -0.53499762f, -0.52458968f,
   -0.51410274f, -0.50353838f, -0.49289819f, -0.48218377f, -0.47139674f,
   -0.46053871f, -0.44961133f, -0.43861624f, -0.42755509f, -0.41642956f,
   -0.40524131f, -0.39399204f, -0.38268343f, -0.37131719f, -0.35989504f,
   -0.34841868f, -0.33688985f, -0.32531029f, -0.31368174f, -0.30200595f,
   -0.29028468f, -0.27851969f, -0.26671276f, -0.25486566f, -0.24298018f,
   -0.23105811f, -0.21910124f, -0.20711138f, -0.19509032f, -0.18303989f,
   -0.17096189f, -0.15885814f, -0.14673047f, -0.13458071f, -0.12241068f,
   -0.11022221f, -0.09801714f, -0.08579731f, -0.07356456f, -0.06132074f,
   -0.04906767f, -0.03680722f, -0.02454123f, -0.01227154f, -0.00000000f
};


#define ATAN_TABLE_SIZE 256
#define ATAN_TABLE_MASK (ATAN_TABLE_SIZE - 1)

// 预计算的atan查找表 [0, 1] -> [0, π/4]
static const float atan_table[ATAN_TABLE_SIZE] = {
    0.000000f, 0.003906f, 0.007812f, 0.011718f, 0.015623f, 0.019528f, 0.023432f, 0.027335f,
    0.031237f, 0.035138f, 0.039038f, 0.042936f, 0.046834f, 0.050729f, 0.054623f, 0.058516f,
    0.062407f, 0.066296f, 0.070183f, 0.074068f, 0.077951f, 0.081832f, 0.085711f, 0.089588f,
    0.093462f, 0.097335f, 0.101205f, 0.105073f, 0.108938f, 0.112801f, 0.116662f, 0.120520f,
    0.124375f, 0.128228f, 0.132078f, 0.135925f, 0.139770f, 0.143611f, 0.147450f, 0.151286f,
    0.155119f, 0.158949f, 0.162776f, 0.166600f, 0.170421f, 0.174239f, 0.178053f, 0.181864f,
    0.185672f, 0.189477f, 0.193278f, 0.197076f, 0.200870f, 0.204661f, 0.208448f, 0.212232f,
    0.216012f, 0.219789f, 0.223562f, 0.227331f, 0.231096f, 0.234858f, 0.238616f, 0.242370f,
    0.246120f, 0.249866f, 0.253608f, 0.257347f, 0.261081f, 0.264811f, 0.268537f, 0.272259f,
    0.275977f, 0.279691f, 0.283400f, 0.287105f, 0.290806f, 0.294502f, 0.298194f, 0.301882f,
    0.305565f, 0.309244f, 0.312918f, 0.316588f, 0.320253f, 0.323914f, 0.327570f, 0.331222f,
    0.334869f, 0.338511f, 0.342149f, 0.345782f, 0.349410f, 0.353033f, 0.356652f, 0.360266f,
    0.363875f, 0.367479f, 0.371078f, 0.374672f, 0.378261f, 0.381845f, 0.385424f, 0.388998f,
    0.392567f, 0.396131f, 0.399690f, 0.403244f, 0.406793f, 0.410336f, 0.413875f, 0.417408f,
    0.420936f, 0.424459f, 0.427976f, 0.431488f, 0.434995f, 0.438496f, 0.441992f, 0.445483f,
    0.448968f, 0.452448f, 0.455922f, 0.459391f, 0.462854f, 0.466312f, 0.469765f, 0.473212f,
    0.476653f, 0.480089f, 0.483519f, 0.486944f, 0.490363f, 0.493777f, 0.497185f, 0.500587f,
    0.503984f, 0.507375f, 0.510760f, 0.514140f, 0.517514f, 0.520882f, 0.524245f, 0.527602f,
    0.530953f, 0.534299f, 0.537639f, 0.540973f, 0.544302f, 0.547624f, 0.550941f, 0.554252f,
    0.557558f, 0.560857f, 0.564151f, 0.567439f, 0.570721f, 0.573997f, 0.577268f, 0.580532f,
    0.583791f, 0.587044f, 0.590291f, 0.593532f, 0.596767f, 0.599996f, 0.603219f, 0.606436f,
    0.609647f, 0.612852f, 0.616051f, 0.619244f, 0.622431f, 0.625612f, 0.628787f, 0.631956f,
    0.635119f, 0.638276f, 0.641427f, 0.644572f, 0.647711f, 0.650844f, 0.653971f, 0.657092f,
    0.660207f, 0.663316f, 0.666419f, 0.669516f, 0.672607f, 0.675692f, 0.678771f, 0.681844f,
    0.684911f, 0.687972f, 0.691027f, 0.694076f, 0.697119f, 0.700156f, 0.703187f, 0.706212f,
    0.709231f, 0.712244f, 0.715251f, 0.718252f, 0.721247f, 0.724236f, 0.727219f, 0.730196f,
    0.733167f, 0.736132f, 0.739091f, 0.742044f, 0.744991f, 0.747932f, 0.750867f, 0.753796f,
    0.756719f, 0.759636f, 0.762547f, 0.765452f, 0.768351f, 0.771244f, 0.774131f, 0.777012f,
    0.779887f, 0.782756f, 0.785619f, 0.788476f, 0.791327f, 0.794172f, 0.797011f, 0.799844f,
    0.802671f, 0.805492f, 0.808307f, 0.811116f, 0.813919f, 0.816716f, 0.819507f, 0.822292f,
    0.825071f, 0.827844f, 0.830611f, 0.833372f, 0.836127f, 0.838876f, 0.841619f, 0.844356f,
    0.847087f, 0.849812f, 0.852531f, 0.855244f, 0.857951f, 0.860652f, 0.863347f, 0.866036f
};

static const unsigned char crc8_table[256] =
{
    0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83,
    0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41,
    0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e,
    0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc,
    0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0,
    0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62,
    0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d,
    0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff,
    0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5,
    0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07,
    0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58,
    0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a,
    0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6,
    0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24,
    0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b,
    0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9,
    0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f,
    0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd,
    0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92,
    0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50,
    0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c,
    0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee,
    0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1,
    0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73,
    0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49,
    0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b,
    0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4,
    0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16,
    0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a,
    0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8,
    0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7,
    0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35,
};

static const unsigned int crc32_table[256] =
{
    0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9,
    0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
    0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
    0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
    0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9,
    0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
    0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011,
    0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd,
    0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
    0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
    0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81,
    0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
    0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49,
    0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
    0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
    0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d,
    0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae,
    0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
    0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16,
    0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca,
    0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
    0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02,
    0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066,
    0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
    0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e,
    0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692,
    0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
    0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a,
    0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e,
    0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
    0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686,
    0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a,
    0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
    0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
    0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f,
    0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
    0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47,
    0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b,
    0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
    0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
    0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7,
    0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
    0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f,
    0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
    0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
    0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b,
    0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f,
    0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
    0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640,
    0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
    0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
    0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24,
    0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30,
    0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
    0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088,
    0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654,
    0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
    0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c,
    0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18,
    0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
    0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0,
    0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c,
    0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
    0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
};

static const uint16_t crc16_table[256] = {
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
    0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
    0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
    0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
    0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
    0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
    0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
    0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
    0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
    0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
    0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
    0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
    0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
    0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
    0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
    0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};


static uint8_t crc8_table_static[256] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04,
    0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8,
    0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10,
    0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C,
    0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0,
    0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C,
    0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54,
    0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98,
    0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};


void APP_Math_CRC8_GenerateTable(void) 
{
    for (int dividend = 0; dividend < 256; dividend++) {
        uint8_t crc = dividend;
        
        for (int bit = 0; bit < 8; bit++) {
            if (crc & 0x01) {
                crc = (crc >> 1) ^ 0x80;
            } else {
                crc >>= 1;
            }
        }
        
        crc8_table_static[dividend] = crc;
    }
}

/**
 * @brief 使用静态查表法计算CRC8（无需生成表）
 */
uint8_t APP_Math_CRC8_StaticTable(uint8_t *message, uint8_t len) 
{
    uint8_t crc = 0x00;
    
    while (len--) {
        uint8_t table_index = crc ^ *message++;
        crc = crc8_table_static[table_index];
    }
    
    return crc;
}
void clarke_transform(float Ia, float Ib, float Ic, float *Ialpha, float *Ibeta)
{
    *Ialpha = Ia;
    *Ibeta  = (Ib - Ic) * ONE_BY_SQRT3;
}

void park_transform(float Ialpha, float Ibeta, float Theta, float *Id, float *Iq)
{
    float s = sin_f32(Theta);
    float c = cos_f32(Theta);
    *Id =   Ialpha * c + Ibeta * s;
    *Iq = - Ialpha * s + Ibeta * c;
}

void inverse_park(float mod_d, float mod_q, float Theta, float *mod_alpha, float *mod_beta)
{
    float s = sin_f32(Theta);
    float c = cos_f32(Theta);
    *mod_alpha = mod_d * c - mod_q * s;
    *mod_beta  = mod_d * s + mod_q * c;
}

int svm(float alpha, float beta, float* tA, float* tB, float* tC)
{
    int Sextant;

    if (beta >= 0.0f) {
        if (alpha >= 0.0f) {
            //quadrant I
            if (ONE_BY_SQRT3 * beta > alpha)
                Sextant = 2; //sextant v2-v3
            else
                Sextant = 1; //sextant v1-v2

        } else {
            //quadrant II
            if (-ONE_BY_SQRT3 * beta > alpha)
                Sextant = 3; //sextant v3-v4
            else
                Sextant = 2; //sextant v2-v3
        }
    } else {
        if (alpha >= 0.0f) {
            //quadrant IV
            if (-ONE_BY_SQRT3 * beta > alpha)
                Sextant = 5; //sextant v5-v6
            else
                Sextant = 6; //sextant v6-v1
        } else {
            //quadrant III
            if (ONE_BY_SQRT3 * beta > alpha)
                Sextant = 4; //sextant v4-v5
            else
                Sextant = 5; //sextant v5-v6
        }
    }

    switch (Sextant) {
        // sextant v1-v2
        case 1: {
            // Vector on-times
            float t1 = alpha - ONE_BY_SQRT3 * beta;
            float t2 = TWO_BY_SQRT3 * beta;

            // PWM timings
            *tA = (1.0f - t1 - t2) * 0.5f;
            *tB = *tA + t1;
            *tC = *tB + t2;
        } break;

        // sextant v2-v3
        case 2: {
            // Vector on-times
            float t2 = alpha + ONE_BY_SQRT3 * beta;
            float t3 = -alpha + ONE_BY_SQRT3 * beta;

            // PWM timings
            *tB = (1.0f - t2 - t3) * 0.5f;
            *tA = *tB + t3;
            *tC = *tA + t2;
        } break;

        // sextant v3-v4
        case 3: {
            // Vector on-times
            float t3 = TWO_BY_SQRT3 * beta;
            float t4 = -alpha - ONE_BY_SQRT3 * beta;

            // PWM timings
            *tB = (1.0f - t3 - t4) * 0.5f;
            *tC = *tB + t3;
            *tA = *tC + t4;
        } break;

        // sextant v4-v5
        case 4: {
            // Vector on-times
            float t4 = -alpha + ONE_BY_SQRT3 * beta;
            float t5 = -TWO_BY_SQRT3 * beta;

            // PWM timings
            *tC = (1.0f - t4 - t5) * 0.5f;
            *tB = *tC + t5;
            *tA = *tB + t4;
        } break;

        // sextant v5-v6
        case 5: {
            // Vector on-times
            float t5 = -alpha - ONE_BY_SQRT3 * beta;
            float t6 = alpha - ONE_BY_SQRT3 * beta;

            // PWM timings
            *tC = (1.0f - t5 - t6) * 0.5f;
            *tA = *tC + t5;
            *tB = *tA + t6;
        } break;

        // sextant v6-v1
        case 6: {
            // Vector on-times
            float t6 = -TWO_BY_SQRT3 * beta;
            float t1 = alpha + ONE_BY_SQRT3 * beta;

            // PWM timings
            *tA = (1.0f - t6 - t1) * 0.5f;
            *tC = *tA + t1;
            *tB = *tC + t6;
        } break;
    }

    // if any of the results becomes NaN, result_valid will evaluate to false
    int result_valid =
            *tA >= 0.0f && *tA <= 1.0f
         && *tB >= 0.0f && *tB <= 1.0f
         && *tC >= 0.0f && *tC <= 1.0f;
    
    return result_valid ? 0 : -1;
}

float sin_f32(float x)
{
    float sinVal, fract, in;                           /* Temporary variables for input, output */
    uint16_t index;                                    /* Index variable */
    float a, b;                                        /* Two nearest output values */
    int32_t n;
    float findex;

    /* input x is in radians */
    /* Scale the input to [0 1] range from [0 2*PI] , divide input by 2*pi */
    in = x * 0.159154943092f;

    /* Calculation of floor value of input */
    n = (int32_t) in;

    /* Make negative values towards -infinity */
    if (x < 0.0f){
        n--;
    }

    /* Map input value to [0 1] */
    in = in - (float) n;

    /* Calculation of index of the table */
    findex = (float)SIN_TABLE_SIZE * in;
    index = (uint16_t)findex;

    /* when "in" is exactly 1, we need to rotate the index down to 0 */
    if (index >= SIN_TABLE_SIZE) {
        index = 0;
        findex -= (float)SIN_TABLE_SIZE;
    }

    /* fractional value calculation */
    fract = findex - (float) index;

    /* Read two nearest values of input value from the sin table */
    a = sinTable_f32[index];
    b = sinTable_f32[index+1];

    /* Linear interpolation process */
    sinVal = (1.0f-fract)*a + fract*b;

    /* Return the output value */
    return (sinVal);
}

float cos_f32(float x)
{
    float cosVal, fract, in;                   /* Temporary variables for input, output */
    uint16_t index;                                /* Index variable */
    float a, b;                                /* Two nearest output values */
    int32_t n;
    float findex;

    /* input x is in radians */
    /* Scale the input to [0 1] range from [0 2*PI] , divide input by 2*pi, add 0.25 (pi/2) to read sine table */
    in = x * 0.159154943092f + 0.25f;

    /* Calculation of floor value of input */
    n = (int32_t) in;

    /* Make negative values towards -infinity */
    if (in < 0.0f){
        n--;
    }

    /* Map input value to [0 1] */
    in = in - (float) n;

    /* Calculation of index of the table */
    findex = (float)SIN_TABLE_SIZE * in;
    index = (uint16_t)findex;

    /* when "in" is exactly 1, we need to rotate the index down to 0 */
    if (index >= SIN_TABLE_SIZE) {
        index = 0;
        findex -= (float)SIN_TABLE_SIZE;
    }

    /* fractional value calculation */
    fract = findex - (float) index;

    /* Read two nearest values of input value from the cos table */
    a = sinTable_f32[index];
    b = sinTable_f32[index+1];

    /* Linear interpolation process */
    cosVal = (1.0f-fract)*a + fract*b;

    /* Return the output value */
    return (cosVal);
}

float atan_f32(float y, float x) 
{
    // 处理特殊情况
    if (x == 0.0f && y == 0.0f) return 0.0f;
    if (x == 0.0f) return (y > 0.0f) ? 1.5707963f : -1.5707963f; // ±π/2
    
    float abs_y = (y < 0.0f) ? -y : y;
    float abs_x = (x < 0.0f) ? -x : x;
    float angle;
    
    if (abs_x >= abs_y) {
        // |x| >= |y|，使用 atan(y/x)
        float ratio = abs_y / abs_x;
        
        // 查表索引，使用位运算优化
        int index = (int)(ratio * (ATAN_TABLE_SIZE - 1));
        index &= ATAN_TABLE_MASK; // 等价于 index % ATAN_TABLE_SIZE
        
        angle = atan_table[index];
        
        // 根据x的符号调整象限
        if (x < 0.0f) {
            angle = 3.1415927f - angle; // π - angle
        }
    } else {
        // |y| > |x|，使用 π/2 - atan(x/y)
        float ratio = abs_x / abs_y;
        
        int index = (int)(ratio * (ATAN_TABLE_SIZE - 1));
        index &= ATAN_TABLE_MASK;
        
        angle = 1.5707963f - atan_table[index]; // π/2 - atan_table[index]
        
        // 根据x的符号调整象限
        if (x < 0.0f) {
            angle = 3.1415927f - angle; // π - angle
        }
    }
    
    // 处理y的符号
    return (y < 0.0f) ? -angle : angle;
}

uint8_t crc8(const uint8_t *data, const uint32_t size)
{
    uint8_t crc = 0;
    for ( uint32_t i = 0; i < size; ++i ) {
         crc = crc8_table[(data[i] ^ crc) & 0xFF];
    }
    return crc;
}

uint32_t crc32(const uint8_t *data, uint32_t size)
{
    uint32_t crc = 0;
    for ( uint32_t i = 0; i < size; ++i ) {
        crc = (crc << 8) ^ crc32_table[((crc >> 24) ^ data[i]) & 0xFF];
    }
    return crc;
}

int uint32_to_data(uint32_t val, uint8_t *data)
{
    data[0] = *(((uint8_t*)(&val)) + 0);
    data[1] = *(((uint8_t*)(&val)) + 1);
    data[2] = *(((uint8_t*)(&val)) + 2);
    data[3] = *(((uint8_t*)(&val)) + 3);
    return 4;
}

int int32_to_data(int32_t val, uint8_t *data)
{
    data[0] = *(((uint8_t*)(&val)) + 0);
    data[1] = *(((uint8_t*)(&val)) + 1);
    data[2] = *(((uint8_t*)(&val)) + 2);
    data[3] = *(((uint8_t*)(&val)) + 3);
    return 4;
}

int uint16_to_data(uint16_t val, uint8_t *data)
{
    data[0] = *(((uint8_t*)(&val)) + 0);
    data[1] = *(((uint8_t*)(&val)) + 1);
    return 2;
}

int int16_to_data(int16_t val, uint8_t *data)
{
    data[0] = *(((uint8_t*)(&val)) + 0);
    data[1] = *(((uint8_t*)(&val)) + 1);
    return 2;
}

int float_to_data(float val, uint8_t *data)
{
    data[0] = *(((uint8_t*)(&val)) + 0);
    data[1] = *(((uint8_t*)(&val)) + 1);
    data[2] = *(((uint8_t*)(&val)) + 2);
    data[3] = *(((uint8_t*)(&val)) + 3);
    return 4;
}

uint32_t data_to_uint32(uint8_t *data)
{
    uint32_t tmp;
    *(((uint8_t*)(&tmp)) + 0) = data[0];
    *(((uint8_t*)(&tmp)) + 1) = data[1];
    *(((uint8_t*)(&tmp)) + 2) = data[2];
    *(((uint8_t*)(&tmp)) + 3) = data[3];
    return tmp;
}

int32_t data_to_int32(uint8_t *data)
{
    int32_t tmp;
    *(((uint8_t*)(&tmp)) + 0) = data[0];
    *(((uint8_t*)(&tmp)) + 1) = data[1];
    *(((uint8_t*)(&tmp)) + 2) = data[2];
    *(((uint8_t*)(&tmp)) + 3) = data[3];
    return tmp;
}

uint16_t data_to_uint16(uint8_t *data)
{
    uint16_t tmp;
    *(((uint8_t*)(&tmp)) + 0) = data[0];
    *(((uint8_t*)(&tmp)) + 1) = data[1];
    return tmp;
}

int16_t data_to_int16(uint8_t *data)
{
    int16_t tmp;
    *(((uint8_t*)(&tmp)) + 0) = data[0];
    *(((uint8_t*)(&tmp)) + 1) = data[1];
    return tmp;
}

float data_to_float(uint8_t *data)
{
    float tmp_float;
    *(((uint8_t*)(&tmp_float)) + 0) = data[0];
    *(((uint8_t*)(&tmp_float)) + 1) = data[1];
    *(((uint8_t*)(&tmp_float)) + 2) = data[2];
    *(((uint8_t*)(&tmp_float)) + 3) = data[3];
    return tmp_float;
}
int float_to_uint(float x, float x_min, float x_max, int bits)
{
    /// Converts a float to an unsigned int, given range and number of bits ///
    float span = x_max - x_min;
    float offset = x_min;
    return (int) ((x-offset)*((float)((1<<bits)-1))/span);
}
    
    
float uint_to_float(int x_int, float x_min, float x_max, int bits)
{
    /// converts unsigned int to float, given range and number of bits ///
    float span = x_max - x_min;
    float offset = x_min;
    return ((float)x_int)*span/((float)((1<<bits)-1)) + offset;
}

uint8_t APP_Math_CRC8_ChkValue(uint8_t *message, uint8_t len) 
{
    uint8_t crc_val = 0;
    uint8_t i;
    while (len--) {
        crc_val ^= *message++;
        for (i = 0; i < 8; i++) {
            if (crc_val & 0x01){
                crc_val = (crc_val >> 1) ^ 0x80;
            }
            else{
                crc_val >>= 1;
            }
        }
    }
    return crc_val;
}

uint16_t ModbusCRC16_Table(uint8_t *data, uint16_t length) 
{
    uint16_t crc = 0xFFFF;
    
    for (uint16_t i = 0; i < length; i++) {
        uint8_t index = (crc ^ data[i]) & 0xFF;
        crc = (crc >> 8) ^ crc16_table[index];
    }
    
    return crc;
}

uint8_t calculate_checksum(const uint8_t *data, uint8_t data_len) 
{
    uint16_t sum = 0;
    
    for (int i = 0; i < data_len; i++) {
        sum += data[i];
    }
    
    uint8_t checksum = ~(sum & 0xFF) + 1;
    
    return checksum;
}

float float_swap_endian(float value) 
{
    union {
        float f;
        uint32_t u32;
    } src, dst;
    
    src.f = value;
    dst.u32 = ((src.u32 & 0x000000FF) << 24) |
              ((src.u32 & 0x0000FF00) << 8)  |
              ((src.u32 & 0x00FF0000) >> 8)  |
              ((src.u32 & 0xFF000000) >> 24);
    
    return dst.f;
}


