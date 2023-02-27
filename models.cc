#include <vector>
#include <memory>

#include "models.h"

// ---------- Model definitions ----------

std::vector<std::shared_ptr<Triangle>> Models::square(glm::vec3 tl, glm::vec3 tr, glm::vec3 br, glm::vec3 bl, const glm::vec3& color){
    auto t1 = std::make_shared<Triangle>(
            tl,     // Top right
            tr,     // Bottom right
            bl,      // Bottom left
            color);

    glm::vec3 color2 = color;
    auto t2 = std::make_shared<Triangle>(
        bl,     // Top left
        tr,     // Top right
        br,      // Bottom left
        color2);
    
    return {t1, t2};
}

std::vector<std::shared_ptr<Triangle>> Models::createBox(glm::vec3 center, float w){
    glm::vec3 ttl = center + glm::vec3(-w, -w, w);   // TL
    glm::vec3 ttr = center + glm::vec3(w, -w, w);   // TR
    glm::vec3 tbr = center + glm::vec3(w, -w, -w);     // BR
    glm::vec3 tbl = center + glm::vec3(-w, -w,-w);    // BL

    // B (y=w)
    glm::vec3 btl = center + glm::vec3(-w, w, w);   // TL
    glm::vec3 btr = center + glm::vec3(w, w, w);   // TR
    glm::vec3 bbr = center + glm::vec3(w, w, -w);     // BR
    glm::vec3 bbl = center + glm::vec3(-w, w,-w);    // BL


    auto top     = square(ttl, ttr, tbr, tbl, Colors::green);  // Green
    auto bottom  = square(btl, bbl, bbr, btr, Colors::red);  // Red 

    auto left    = square(ttl, tbl, bbl, btl, Colors::blue);  // Blue +
    auto right   = square(ttr, btr, bbr, tbr, Colors::yellow);  // Yellow
    
    auto back    = square(ttl, btl, btr, ttr, Colors::magenta);  // Magenta +
    auto front   = square(tbl, tbr, bbr, bbl, Colors::cyan);  // Cyan +

    return {
        top[0], top[1], 
        bottom[0], bottom[1], 
        left[0], left[1],
        right[0], right[1], 
        back[0], back[1], 
        front[0], front[1]
    };
}
