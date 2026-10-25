#include "inventory/SparePart.h"
#include "devices/Device.h"

SparePart::SparePart(const std::string& article, const std::string& name,
                     const std::vector<std::string>& models, const Money& buyPrice, const Money& sellPrice)
    : article_(article), name_(name), models_(models), buyPrice_(buyPrice), sellPrice_(sellPrice) {}

bool SparePart::fits(const Device& d) const {
    for (size_t i = 0; i < models_.size(); ++i)
        if (models_[i] == d.getModel()) return true;
    return false;
}

void SparePart::addModel(const std::string& model) {
    for (size_t i = 0; i < models_.size(); ++i)
        if (models_[i] == model) return;
    models_.push_back(model);
}

Money SparePart::margin() const {
    return sellPrice_ - buyPrice_;
}

double SparePart::marginPercent() const {
    if (buyPrice_.isZero()) return 0;
    return margin().toRubles() / buyPrice_.toRubles() * 100;
}

const std::string& SparePart::getArticle() const { return article_; }

const std::string& SparePart::getName() const { return name_; }

const std::vector<std::string>& SparePart::getModels() const { return models_; }

const Money& SparePart::getBuyPrice() const { return buyPrice_; }

const Money& SparePart::getSellPrice() const { return sellPrice_; }

bool SparePart::operator==(const SparePart& other) const { return article_ == other.article_; }

bool SparePart::operator!=(const SparePart& other) const { return !(*this == other); }

std::ostream& operator<<(std::ostream& os, const SparePart& p) {
    return os << p.article_ << " " << p.name_ << ", моделей: " << p.models_.size()
              << ", цена " << p.sellPrice_;
}
