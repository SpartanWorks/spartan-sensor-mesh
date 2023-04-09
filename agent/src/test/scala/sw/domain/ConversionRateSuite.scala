package sw.domain

import cats.data.*
import cats.data.Validated.{Invalid, Valid}
import cats.implicits.*
import cats.syntax.apply.*
import io.circe.*
import io.circe.jawn.*
import io.circe.syntax.*
import io.circe.generic.semiauto.*

import sw.domain.ConversionRate
import sw.domain.ConversionRate.given

class ConversionRateSuite extends munit.FunSuite {

  test("ConversionRate should correctly validate fields") {
    ConversionRate.create("XYZ", "EUR", -0.5) match {
      case Invalid(errors) =>
        assert(errors.toList.contains("Invalid currency code: XYZ"))
        assert(errors.toList.contains("Rate should not be negative"))
      case Valid(_) => fail("Validated invalid ConversionRate")
    }
  }

  test("ConversionRate encoder should generate correct JSON") {
    val conversionRate = ConversionRate("USD", "EUR", 0.85)
    assertEquals(conversionRate.asJson.noSpaces, """{"sourceCurrency":"USD","destinationCurrency":"EUR","rate":0.85}""")
  }

  test("ConversionRate decoder should correctly decode valid JSON") {
    val validJson = """{"sourceCurrency":"USD","destinationCurrency":"EUR","rate":0.85}"""
    val expectedConversionRate = ConversionRate("USD", "EUR", 0.85)
    val decodedConversionRate = decode[ValidatedNec[String, ConversionRate]](validJson)
    decodedConversionRate match {
      case Left(_) => fail("Failed to decode valid JSON")
      case Right(Valid(actualConversionRate)) => assertEquals(actualConversionRate, expectedConversionRate)
      case Right(Invalid(errors)) => fail(s"Decoded JSON but got unexpected errors: ${errors.toList}")
    }
  }

  test("ConversionRate decoder should correctly handle invalid JSON") {
    val invalidJson = """{"sourceCurrency":"USD","destinationCurrency":"EUR","rate":"invalid"}"""
    val decodedConversionRate = decode[ValidatedNec[String, ConversionRate]](invalidJson)
    decodedConversionRate match {
      case Left(_) => assert(true)
      case Right(Valid(actualConversionRate)) => fail("Decoded invalid JSON")
      case Right(Invalid(errors)) => fail("Decoded invalid JSON")
    }
  }

  test("ConversionRate decoder should correctly validate ConversionRate object fields") {
    val invalidJson = """{"sourceCurrency":"XYZ","destinationCurrency":"ABC","rate":-0.5}"""
    val decodedConversionRate = decode[ValidatedNec[String, ConversionRate]](invalidJson)
    decodedConversionRate match {
      case Left(_) => fail("Failed to decode valid JSON")
      case Right(Valid(actualConversionRate)) => fail("Decoded invalid ConversionRate")
      case Right(Invalid(errors)) =>
        assertEquals(errors, NonEmptyChain(
          "Invalid currency code: XYZ",
          "Invalid currency code: ABC",
          "Rate should not be negative",
        ))
    }
  }

}
