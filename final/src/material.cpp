#include "material.h"

pdf* MaterialBase::sample_brdf( Vector3 wi, Vector3& wo, scatter_record& srec, pcg32_state rng) {
    pdf* sampled_pdf = nullptr;
    return sampled_pdf; // for mirror
}

Vector3 MaterialBase::eval_brdf( Vector3 wi, Vector3 wo, pdf& pdf, bool nexthitlight) {
    Vector3 brdf_value{0.,0.,0.};
    return brdf_value;
}

pdf* Diffuse::sample_brdf( Vector3 wi, Vector3& wo, scatter_record& srec, pcg32_state rng) {
    srec.is_specular = false;
    auto n =rec->normal;
    auto cosinep = new cosine_pdf(n,rng);
    wo = cosinep->generate();
    return cosinep;
}

Vector3 Diffuse::eval_brdf( Vector3 wi, Vector3 wo, pdf& pdf, bool nexthitlight) {
    auto p = rec->position;
    return getAlbedo() * pdf.value(wo,p,nexthitlight);
}

pdf* Mirror::sample_brdf( Vector3 wi, Vector3& wo, scatter_record& srec, pcg32_state rng)  {
    srec.is_specular = true;
    auto n = rec->normal;
    wo = normalize(2. * dot(wi, n) * n  - wi); // n: (x = 0, y = 3.049329269387672E-314, z = 0) 03:44 May26
    srec.sampling_weight = 1.; // do I need to change? I think so
    return nullptr;
}

pdf* Plastic::sample_brdf( Vector3 wi, Vector3& wo, scatter_record& srec, pcg32_state rng)  {
    srec.is_specular = false;
    // pcg32_state rng_ = init_pcg32(time(NULL));
    auto r0 = next_pcg32_real<Real>(rng);//static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    auto n = rec->normal;
    auto r = normalize(2. * dot(wi,n) * n - wi);
    auto cosinep = new cosine_pdf(n,rng);
    auto F = plastic_frasnel(wi);
    wo = r0 < 2 * F? r : cosinep->generate();
    return cosinep;
}

Vector3 Plastic::eval_brdf( Vector3 wi, Vector3 wo, pdf& pdf, bool nexthitlight)  {
    auto p = rec->position;
    return getAlbedo() * pdf.value(wo,p, nexthitlight);
}

pdf* Phong::sample_brdf( Vector3 wi, Vector3& wo, scatter_record& srec, pcg32_state rng) {
    srec.is_specular = false;
    auto n = rec->normal;
    auto r = -wi + 2 * dot(wi, n) * n;
    auto phongp = new phong_pdf(r, exponent, rng);
    wo = phongp->generate();
    return phongp;
}

Vector3 Phong::eval_brdf( Vector3 wi, Vector3 wo, pdf& pdf, bool nexthitlight)  {
    auto n = rec->normal;
    auto r = -wi + 2 * dot(wi, n) * n;
    return dot(n,wo) <= 0 ? Vector3(0,0,0) : getAlbedo() * (exponent + 1) / (2.0 * c_PI) * pow(max(dot(r, wo),Real(0)), exponent) ;
}

pdf* BlinnPhong::sample_brdf( Vector3 wi, Vector3& wo, scatter_record& srec, pcg32_state rng)  {
    auto n = rec->normal;
    auto blinnp = new blinn_pdf(n,exponent,rng);
    blinnp->h = blinnp->generate();
    wo = 2. * dot(blinnp->h, wi)* blinnp->h  - wi;
    return blinnp;
}

Vector3 BlinnPhong::eval_brdf( Vector3 wi, Vector3 wo, pdf& pdf, bool nexthitlight)  {
    auto n = rec->normal;
    auto h = normalize((wi + wo) / 2.);
    auto F_h = eval_frasnel(wo,h);
    auto brdf_value = dot(n,wo) <= 0 ? Vector3(0,0,0) : F_h * pow(dot(n, h), exponent) ; 
    return brdf_value * (exponent + 2) 
            / (4. * c_PI * (2. - pow(2, -exponent/2.)));
}

pdf* BlinnPhongMicrofacet::sample_brdf( Vector3 wi, Vector3& wo, scatter_record& srec, pcg32_state rng)  {
    auto n = rec->normal;
    auto blinnp = new blinn_pdf(n,exponent, rng);
    blinnp->h = blinnp->generate();
    wo = 2. * dot(blinnp->h, wi)* blinnp->h  - wi;
    return blinnp;
}

Vector3 BlinnPhongMicrofacet::eval_brdf( Vector3 wi, Vector3 wo, pdf& pdf, bool nexthitlight)  {
    auto n = rec->normal;
    auto h = normalize((wi + wo) / 2.);
    auto F_h = eval_frasnel(wo,h);
    Real D_value = D(n,h);
    Real G_value = G1_hat(n,wi) * G1_hat(n,wo);
    return dot(n,wo) <= 0? Vector3(0,0,0): F_h * D_value * G_value / 
                                            (4. * dot(n,wi));
}