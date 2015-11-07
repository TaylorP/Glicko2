#include "glicko/rating.hpp"

using namespace Glicko;

Rating::Rating(const double rating,
               const double deviation,
               const double volatility)
{
    u = (rating - Glicko::kDefaultR) / Glicko::kScale;
    p = deviation / Glicko::kScale;
    s = volatility;
}

Rating::Rating(const Rating& rating)
{
    u = rating.u;
    p = rating.p;
    s = rating.s;
}

void Rating::Update(const int m, const Rating* opponents, const double* score)
{
    double gTable[m];
    double eTable[m];
    double invV = 0.0;

    // Compute the g and e values for each opponent and 
    // accumulate the results into the v value
    for (int j = 0; j < m; j++)
    {
        const Rating& opponent = opponents[j];

        double g = opponent.G();
        double e = opponent.E(g, *this);

        gTable[j] = g;
        eTable[j] = e;
        invV += g * g * e * (1.0 - e); 
    }

    // Invert the v value
    double v = 1.0 / invV;

    // Compute the delta value from the g, e, and v
    // values
    double dInner = 0.0;
    for (int j = 0; j < m; j++)
    {
        dInner += gTable[j] * (score[j] - eTable[j]);
    }

    // Apply the v value to the delta
    double d = v * dInner;

    // Compute new rating, deviation and volatility values
    sPrime = exp(Convergence(d, v, p, s)/2.0);
    pPrime = 1.0 / sqrt((1.0/(p*p + sPrime*sPrime)) + invV);
    uPrime = u + pPrime * pPrime * dInner;
}

void Rating::Update(const Rating& opponent, const double score)
{
    // Compute the e and g function values
    double g = opponent.G();
    double e = opponent.E(g, *this);

    // Compute 1/v and v
    double invV = g * g * e * (1.0 - e);
    double v = 1.0 / invV;

    // Compute the delta value from the g, e, and v
    // values
    double dInner = g * (score - e);
    double d = v * dInner;

    // Compute new rating, deviation and volatility values
    sPrime = exp(Convergence(d, v, p, s)/2.0);
    pPrime = 1.0 / sqrt((1.0/(p*p + sPrime*sPrime)) + invV);
    uPrime = u + pPrime * pPrime * dInner;
}

void Rating::Decay()
{
    // Decay the deviation if no games were played
    pPrime = sqrt(p*p + s*s);
}

void Rating::Apply()
{
    // Assign the new pending values to the actual rating values
    u = uPrime;
    p = pPrime;
    s = sPrime;
}

double Rating::Convergence(const double d,
                           const double v,
                           const double p,
                           const double s)
{
    // Initialize function values for iteration procedure
    double dS = d*d;
    double pS = p*p;
    double tS = Glicko::kSystemConst * Glicko::kSystemConst;
    double a = log(s*s);

    // Select the upper and lower iteration ranges
    double A = a;
    double B;
    double bTest = dS - pS - v;

    if (bTest > 0.0)
    {
        B = log(bTest);
    }
    else
    {
        B = a - Glicko::kSystemConst;
        while (F(B, dS, pS, v, a, tS) < 0.0)
        {
            B -= Glicko::kSystemConst;
        }
    }

    // Perform the iteration
    double fA = F(A, dS, pS, v, a, tS);
    double fB = F(B, dS, pS, v, a, tS);
    while (fabs(B - A) > Glicko::kConvergence)
    {
        double C = A + (A - B) * fA / (fB - fA);
        double fC = F(C, dS, pS, v, a, tS);

        if (fC * fB < 0.0)
        {
            A = B;
            fA = fB;
        }
        else
        {
            fA /= 2.0;
        }

        B = C;
        fB = fC;
    }

    return A;
}