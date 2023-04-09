package sw.domain

import cats.data.*
import cats.implicits.*
import cats.syntax.apply.*
import io.circe.*
import io.circe.generic.semiauto.*

case class ConversionRate(sourceCurrency: String, destinationCurrency: String, rate: Double)

object ConversionRate:
  def validateRate(rate: Double): ValidatedNec[String, Double] =
    Validated.condNec(rate >= 0, rate, "Rate should not be negative")

  def validateCurrency(currency: String): ValidatedNec[String, String] =
    Validated.condNec(Set("USD", "EUR", "PLN", "XAU").contains(currency), currency, s"Invalid currency code: $currency")

  def create(sourceCurrency: String, destinationCurrency: String, rate: Double): ValidatedNec[String, ConversionRate] = (
    validateCurrency(sourceCurrency),
    validateCurrency(destinationCurrency),
    validateRate(rate),
  ).mapN(ConversionRate.apply)

  inline given Encoder[ConversionRate] = deriveEncoder
  inline given Decoder[ValidatedNec[String, ConversionRate]] =
    Decoder.forProduct3("sourceCurrency", "destinationCurrency", "rate")(create)
